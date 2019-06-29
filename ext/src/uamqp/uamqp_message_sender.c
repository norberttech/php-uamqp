#include <main/php.h>
#include <azure_uamqp_c/uamqp.h>
#include <azure_uamqp_c/messaging.h>
#include <azure_uamqp_c/link.h>
#include <azure_uamqp_c/message_sender.h>
#include <azure_c_shared_utility/gballoc.h>
#include "uamqp_message_sender.h"

int messages_sent = 0;

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

struct uamqp_message_sender create_message_sender(struct uamqp_session uamqp_session, char *host, char *destination)
{
    struct uamqp_message_sender sender;
    AMQP_VALUE source;
    AMQP_VALUE target;
    char uamqpHost[256];

    snprintf(uamqpHost, sizeof uamqpHost, "amqps://%s/%s", host, destination);

    source = messaging_create_source("ingress");
    target = messaging_create_target(uamqpHost);

    sender.link = link_create(uamqp_session.session, "sender-link", role_sender, source, target);
    link_set_snd_settle_mode(sender.link, sender_settle_mode_settled);
    (void)link_set_max_message_size(sender.link , 65536);
    link_subscribe_on_link_detach_received(sender.link, on_link_detach_received_producer, uamqp_session.session);

    amqpvalue_destroy(source);
    amqpvalue_destroy(target);

    sender.message_sender = messagesender_create(sender.link, NULL, NULL);

    if (sender.message_sender == NULL) {
        // todo: throw php error
    }

    return sender;
}

struct uamqp_message create_message(char *content)
{
    struct uamqp_message message;
    BINARY_DATA binary_data;

    message.message = message_create();

    binary_data.bytes = (unsigned char *) content;
    binary_data.length = sizeof(content);
    message_add_body_amqp_data(message.message, binary_data);

    return message;
}

static void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    (void)send_result;
    (void)context;
    (void)delivery_state;

    messages_sent += 1;
}

void send_message(struct uamqp_connection uamqp_connection, struct uamqp_message_sender uamqp_messag_sender, struct uamqp_message uamqp_message)
{
    if (messagesender_open(uamqp_messag_sender.message_sender) != 0) {
        // todo: throw php error
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