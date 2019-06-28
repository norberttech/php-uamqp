#include <stdbool.h>
#include <azure_c_shared_utility/tickcounter.h>
#include <azure_c_shared_utility/tlsio.h>
#include <azure_c_shared_utility/platform.h>
#include <azure_uamqp_c/connection.h>
#include <azure_uamqp_c/saslclientio.h>
#include <azure_uamqp_c/sasl_mssbcbs.h>
#include <azure_uamqp_c/sasl_plain.h>
#include <azure_uamqp_c/session.h>
#include <azure_c_shared_utility/xlogging.h>
#include <azure_uamqp_c/messaging.h>
#include <azure_uamqp_c/link.h>
#include <azure_uamqp_c/message_sender.h>
#include <main/php.h>
#include "uamqp_connection.h"

int messages_sent = 0;

CONNECTION_HANDLE create_uamqp_connection(char *host, long port, char *policyName, char *policyKey)
{
    CONNECTION_HANDLE connection;
    const IO_INTERFACE_DESCRIPTION* tlsio_interface;
    TLSIO_CONFIG tls_io_config = { host, port };
    XIO_HANDLE tls_io;
    SASLCLIENTIO_CONFIG sasl_io_config;
    SASL_MECHANISM_HANDLE sasl_mechanism_handle;
    SASL_PLAIN_CONFIG sasl_plain_config = {policyName, policyKey, NULL };
    XIO_HANDLE sasl_io;
    TICK_COUNTER_HANDLE tickCounterHandle;

    if (platform_init() != 0) {
        // not sure what to do when
    }

    // Create TLS IO
    tlsio_interface = platform_get_default_tlsio();
    tls_io = xio_create(tlsio_interface, &tls_io_config);


    // Create SASL IO
    sasl_mechanism_handle  = saslmechanism_create(saslplain_get_interface(), &sasl_plain_config);
    sasl_io_config.underlying_io = tls_io;
    sasl_io_config.sasl_mechanism = sasl_mechanism_handle;
    sasl_io = xio_create(saslclientio_get_interface_description(), &sasl_io_config);

    // create connection
    tickCounterHandle = tickcounter_create();
    tickcounter_destroy(tickCounterHandle);

    connection = connection_create(sasl_io, host, "php-uaqmp-binding", NULL, NULL);
    connection_set_trace(connection, 1);

    int connectionStatus = connection_open(connection);

    return connection;
}

SESSION_HANDLE create_uamqp_session(CONNECTION_HANDLE connection)
{
    SESSION_HANDLE session;
    session =  session_create(connection, NULL, NULL);
    session_set_incoming_window(session, 2147483647);
    session_set_outgoing_window(session, 65536);

    return session;
}


static void on_link_detach_received_producer(void* context, ERROR_HANDLE error)
{
    (void)error;
    const char* condition = NULL;
    const char* description = NULL;
    error_get_condition(error, &condition);
    error_get_description(error, &description);
    php_printf(condition);
    php_printf(description);

    // todo: throw php error
}

MESSAGE_SENDER_HANDLE create_message_sender(SESSION_HANDLE session, char *host, char *destination)
{
    MESSAGE_SENDER_HANDLE sender;
    AMQP_VALUE source;
    AMQP_VALUE target;
    LINK_HANDLE link;
    char uamqpHost[256];

    snprintf(uamqpHost, sizeof uamqpHost, "amqps://%s/%s", host, destination);

    source = messaging_create_source("ingress");
    target = messaging_create_target(uamqpHost);
    link = link_create(session, "sender-link", role_sender, source, target);
    link_set_snd_settle_mode(link, sender_settle_mode_settled);
    (void)link_set_max_message_size(link, 65536);
    link_subscribe_on_link_detach_received(link, on_link_detach_received_producer, session);

    amqpvalue_destroy(source);
    amqpvalue_destroy(target);

    sender = messagesender_create(link, NULL, NULL);

    if (sender == NULL) {
        // todo: throw php error
    }

    return sender;
}

MESSAGE_HANDLE create_message(char *content)
{
    MESSAGE_HANDLE message;
    BINARY_DATA binary_data;
    message = message_create();

    binary_data.bytes = content;
    binary_data.length = sizeof(content);
    message_add_body_amqp_data(message, binary_data);

    return message;
}

static void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    (void)send_result;
    (void)context;
    (void)delivery_state;

    messages_sent += 1;
}

void send_message(CONNECTION_HANDLE connection, MESSAGE_SENDER_HANDLE message_sender, MESSAGE_HANDLE message)
{
    if (messagesender_open(message_sender) != 0) {
        // todo: throw php error
    }

    (void) messagesender_send_async(message_sender, message, on_message_send_complete, message, 10000);

    while (messages_sent < 1) {
        connection_dowork(connection);
    }
}