#include <string.h>
#include <stdbool.h>
#include <main/php.h>
#include <azure_c_shared_utility/tickcounter.h>
#include <azure_c_shared_utility/tlsio.h>
#include <azure_c_shared_utility/platform.h>
#include <azure_uamqp_c/connection.h>
#include <azure_uamqp_c/saslclientio.h>
#include <azure_uamqp_c/sasl_mssbcbs.h>
#include <azure_uamqp_c/sasl_plain.h>
#include <azure_uamqp_c/session.h>
#include "uamqp.h"
#include "../php/php_uamqp_exception.h"

static void on_connection_closed_received(void* context, ERROR_HANDLE error)
{
    (void) context;
    const char* description = NULL;
    error_get_description(error, &description);

    php_uamqp_throw_exception((char*) description, 0);
}

struct uamqp create_uamqp_connection(char *host, int port, char *policy_name, char *policy_key)
{
    struct uamqp connection;

    const IO_INTERFACE_DESCRIPTION* tlsio_interface;
    TLSIO_CONFIG tls_io_config = { host, port };
    SASLCLIENTIO_CONFIG sasl_io_config;
    SASL_PLAIN_CONFIG sasl_plain_config = {policy_name, policy_key, NULL };

    if (platform_init() != 0) {
        php_uamqp_throw_exception("UAQMP platform already initialized", 0);
    }

    // Create TLS IO
    tlsio_interface = platform_get_default_tlsio();
    connection.tls_io = xio_create(tlsio_interface, &tls_io_config);

    // Create SASL IO
    connection.sasl_mechanism_handle  = saslmechanism_create(saslplain_get_interface(), &sasl_plain_config);
    sasl_io_config.underlying_io = connection.tls_io;
    sasl_io_config.sasl_mechanism = connection.sasl_mechanism_handle;
    connection.sasl_io = xio_create(saslclientio_get_interface_description(), &sasl_io_config);

    if (connection.sasl_io == NULL) {
        php_uamqp_throw_exception("Connection SASL IO Error", 0);
    }

    connection.connection = connection_create(connection.sasl_io, host, "php-uaqmp-binding", NULL, NULL);

    (void) connection_subscribe_on_connection_close_received(connection.connection, on_connection_closed_received, NULL);

    TICK_COUNTER_HANDLE tick_counter = tickcounter_create();
    tickcounter_destroy(tick_counter);

    return connection;
}

void set_uamqp_connection_debug_mode(struct uamqp connection, int enabled)
{
    connection_set_trace(connection.connection, enabled);
}

struct uamqp_session create_uamqp_session(struct uamqp connection)
{
    struct uamqp_session session;

    session.session =  session_create(connection.connection, NULL, NULL);
    session_set_incoming_window(session.session, PHP_UAMQP_MAX_FRAME_SIZE_BYTES);
    session_set_outgoing_window(session.session, PHP_UAMQP_MAX_FRAME_SIZE_BYTES);

    return session;
}

void uamqp_connection_work(struct uamqp uamqp_connection)
{
    connection_dowork(uamqp_connection.connection);
}

void destroy_connection(struct uamqp *uamqp_connection, struct uamqp_session *uamqp_session)
{
    session_destroy(uamqp_session->session);
    connection_destroy(uamqp_connection->connection);
    xio_destroy(uamqp_connection->sasl_io);
    xio_destroy(uamqp_connection->tls_io);
    saslmechanism_destroy(uamqp_connection->sasl_mechanism_handle);

    uamqp_connection->connection = NULL;
    uamqp_connection->tls_io = NULL;
    uamqp_connection->sasl_io = NULL;
    uamqp_connection->sasl_mechanism_handle = NULL;
    uamqp_session->session = NULL;

    platform_deinit();
}

// SENDER

static void on_link_detach_received_producer(void* context, ERROR_HANDLE error)
{
    (void)error;
    const char* condition = NULL;
    const char* description = NULL;
    error_get_condition(error, &condition);
    error_get_description(error, &description);

    php_uamqp_throw_exception((char*) description, 0);
}

struct uamqp_message_sender create_message_sender(struct uamqp_session uamqp_session, char *host, char *destination)
{
    struct uamqp_message_sender sender;
    AMQP_VALUE source;
    AMQP_VALUE target;
    char uamqpHost[1024];

    snprintf(uamqpHost, sizeof uamqpHost, "amqps://%s/%s", host, destination);

    source = messaging_create_source("ingress");
    target = messaging_create_target(uamqpHost);

    sender.link = link_create(uamqp_session.session, "sender-link", role_sender, source, target);
    link_set_snd_settle_mode(sender.link, sender_settle_mode_unsettled);
    (void)link_set_max_message_size(sender.link , PHP_UAMQP_MAX_MESSAGE_LENGTH_BYTES);
    link_subscribe_on_link_detach_received(sender.link, on_link_detach_received_producer, uamqp_session.session);

    amqpvalue_destroy(source);
    amqpvalue_destroy(target);

    sender.message_sender = messagesender_create(sender.link, NULL, NULL);

    if (sender.message_sender == NULL) {
        php_uamqp_throw_exception("Failed to initialize message sender.", 0);
    }

    return sender;
}

struct uamqp_message create_message(char *content)
{
    struct uamqp_message message;
    BINARY_DATA binary_data;

    message.message = message_create();

    binary_data.bytes = (unsigned char *) content;
    binary_data.length = strlen(content);
    message_add_body_amqp_data(message.message, binary_data);

    return message;
}

int messages_sent = 0;

static void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    (void)send_result;
    (void)context;
    (void)delivery_state;

    messages_sent += 1;
}

void send_message(struct uamqp uamqp_connection, struct uamqp_message_sender uamqp_messag_sender, struct uamqp_message uamqp_message)
{
    if (messagesender_open(uamqp_messag_sender.message_sender) != 0) {
        php_uamqp_throw_exception("Failed to open message sender.", 0);
    }

    (void) messagesender_send_async(uamqp_messag_sender.message_sender, uamqp_message.message, on_message_send_complete, uamqp_message.message, 5000);
    message_destroy(uamqp_message.message);

    while (messages_sent == 0) {
        connection_dowork(uamqp_connection.connection);
    }

    messages_sent = 0;

    messagesender_destroy(uamqp_messag_sender.message_sender);
    link_destroy(uamqp_messag_sender.link);
}

// RECEIVER
#define RECEIVE_AND_DELETE 0
#define PEAK_AND_LOCK 1

char * _last_message = NULL;

static AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE value;
    char raw_message[65536] = {};
    BINARY_DATA binary_data;

    message_get_body_amqp_data_in_place(message, 0, &binary_data);
    strncpy(raw_message, (const char *) binary_data.bytes, binary_data.length);

    _last_message = estrdup(raw_message);

    return value;
}

struct uamqp_message_receiver create_message_receiver(struct uamqp_session *uamqp_session, char *host, char *destination, int settle_mode)
{
    struct uamqp_message_receiver receiver;
    AMQP_VALUE source;
    AMQP_VALUE target;
    char uamqpHost[2048];

    snprintf(uamqpHost, sizeof uamqpHost, "amqps://%s/%s", host, destination);

    source = messaging_create_source(uamqpHost);
    target = messaging_create_target("ingress-rx");
    receiver.link = link_create(uamqp_session->session, "receiver-link", role_receiver, source, target); // @TODO: Replace link name with uuid

    // how many messages consumer can handle during one work cycle
    link_set_max_link_credit(receiver.link, 1);

    if (settle_mode == RECEIVE_AND_DELETE) {
        link_set_rcv_settle_mode(receiver.link, receiver_settle_mode_first);
    }

    if (settle_mode == PEAK_AND_LOCK) {
        link_set_rcv_settle_mode(receiver.link, receiver_settle_mode_second);
    } else {
        link_set_rcv_settle_mode(receiver.link, receiver_settle_mode_first); // use receive and delete as a default
    }


    amqpvalue_destroy(source);
    amqpvalue_destroy(target);

    receiver.message_receiver = messagereceiver_create(receiver.link, NULL, NULL);
    receiver.settle_mode = settle_mode;
    receiver.state = RECEIVER_OPEN;

    return receiver;
}

void uamqp_open_receiver(struct uamqp_message_receiver *uamqp_message_receiver)
{
    messagereceiver_open(uamqp_message_receiver->message_receiver, on_message_received, uamqp_message_receiver->message_receiver);
    uamqp_message_receiver->state = RECEIVER_OPEN;
}

char * uamqp_pull_last_message()
{
    return _last_message;
}

void uamqp_close_receiver(struct uamqp_message_receiver *uamqp_message_receiver)
{
    if (_last_message) {
        efree(_last_message);
        _last_message = NULL;
    }

    messagereceiver_destroy(uamqp_message_receiver->message_receiver);
    link_destroy(uamqp_message_receiver->link);
    uamqp_message_receiver->state = RECEIVER_CLOSED;
}

void uamqp_receiver_accept_last_message(struct uamqp_message_receiver *uamqp_message_receiver)
{
    if (_last_message) {
        AMQP_VALUE result = messaging_delivery_accepted();
        delivery_number message_id;
        const char* link_name;

        messagereceiver_get_received_message_id(uamqp_message_receiver->message_receiver, &message_id);
        messagereceiver_get_link_name(uamqp_message_receiver->message_receiver, &link_name);
        messagereceiver_send_message_disposition(uamqp_message_receiver->message_receiver, link_name, message_id, result);

        efree(_last_message);
        _last_message = NULL;
    }
}

void uamqp_receiver_release_last_message(struct uamqp_message_receiver *uamqp_message_receiver)
{
    if (_last_message) {
        AMQP_VALUE result = messaging_delivery_released();
        delivery_number message_id;
        const char* link_name;

        messagereceiver_get_received_message_id(uamqp_message_receiver->message_receiver, &message_id);
        messagereceiver_get_link_name(uamqp_message_receiver->message_receiver, &link_name);
        messagereceiver_send_message_disposition(uamqp_message_receiver->message_receiver, link_name, message_id, result);

        efree(_last_message);
        _last_message = NULL;
    }
}

void uamqp_receiver_reject_last_message(struct uamqp_message_receiver *uamqp_message_receiver, char* error_condition, char* error_description)
{
    if (_last_message) {
        AMQP_VALUE result = messaging_delivery_rejected(error_condition, error_description);
        delivery_number message_id;
        const char* link_name;

        messagereceiver_get_received_message_id(uamqp_message_receiver->message_receiver, &message_id);
        messagereceiver_get_link_name(uamqp_message_receiver->message_receiver, &link_name);
        messagereceiver_send_message_disposition(uamqp_message_receiver->message_receiver, link_name, message_id, result);

        efree(_last_message);
        _last_message = NULL;
    }
}

