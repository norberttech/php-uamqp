#include <azure_uamqp_c/uamqp.h>

struct uamqp {
    CONNECTION_HANDLE connection;
    XIO_HANDLE tls_io;
    XIO_HANDLE sasl_io;
    SASL_MECHANISM_HANDLE sasl_mechanism_handle;
};

struct uamqp_session {
    SESSION_HANDLE session;
};

struct uamqp_message_sender {
    LINK_HANDLE link;
    MESSAGE_SENDER_HANDLE message_sender;
};

struct uamqp_message {
    MESSAGE_HANDLE message;
};

struct uamqp_message_receiver {
    LINK_HANDLE link;
    MESSAGE_RECEIVER_HANDLE message_receiver;
    int settle_mode;
};

struct uamqp create_uamqp_connection(char *host, int port, char *policyName, char *policyKey);
void set_uamqp_connection_debug_mode(struct uamqp connection, int enabled);
struct uamqp_session create_uamqp_session(struct uamqp connection);
void destroy_connection(struct uamqp *uamqp_connection, struct uamqp_session *uamqp_session);

struct uamqp_message_sender create_message_sender(struct uamqp_session uamqp_session, char *host, char *destination);
struct uamqp_message create_message(char *content);
void send_message(struct uamqp uamqp_connection, struct uamqp_message_sender uamqp_messag_sender, struct uamqp_message uamqp_message);

struct uamqp_message_receiver create_message_receiver(struct uamqp_session uamqp_session, char *host, char *destination, int settle_mode);
void uamqp_open_receiver(struct uamqp uamqp_connection, struct uamqp_message_receiver uamqp_message_receiver, int (*callback)(char *msg));

