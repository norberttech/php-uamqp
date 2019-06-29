#include <azure_uamqp_c/uamqp.h>
#include "uamqp_connection.h"

struct uamqp_message_sender {
    LINK_HANDLE link;
    MESSAGE_SENDER_HANDLE message_sender;
};

struct uamqp_message {
    MESSAGE_HANDLE message;
};

struct uamqp_message_sender create_message_sender(struct uamqp_session uamqp_session, char *host, char *destination);
struct uamqp_message create_message(char *content);
void send_message(struct uamqp_connection uamqp_connection, struct uamqp_message_sender uamqp_messag_sender, struct uamqp_message uamqp_message);
