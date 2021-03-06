#include <azure_uamqp_c/uamqp.h>

enum UAMQP_CONSTS {
    PHP_UAMQP_AMQPS_PORT = 5671,
    // PHP_UAMQP_MAX_PARTITION_KEY_LENGTH = 128,
    PHP_UAMQP_MAX_MESSAGE_LENGTH_BYTES = 1048576, //1024 * 1024;
    PHP_UAMQP_MAX_FRAME_SIZE_BYTES = 65536 // 64 * 1024
};

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

enum RECEIVER_STATE {
    RECEIVER_NEW,
    RECEIVER_OPEN,
    RECEIVER_CLOSED
};

struct uamqp_message_receiver {
    LINK_HANDLE link;
    MESSAGE_RECEIVER_HANDLE message_receiver;
    int settle_mode;
    enum RECEIVER_STATE state;
};

struct uamqp create_uamqp_connection(char *host, int port, char *policy_name, char *policy_key);
void set_uamqp_connection_debug_mode(struct uamqp connection, int enabled);
struct uamqp_session create_uamqp_session(struct uamqp connection);
void uamqp_connection_work(struct uamqp uamqp_connection);
void destroy_connection(struct uamqp *uamqp_connection, struct uamqp_session *uamqp_session);

struct uamqp_message_sender create_message_sender(struct uamqp_session uamqp_session, char *host, char *destination);
struct uamqp_message create_message(char *content);
void send_message(struct uamqp uamqp_connection, struct uamqp_message_sender uamqp_messag_sender, struct uamqp_message uamqp_message);

struct uamqp_message_receiver create_message_receiver(struct uamqp_session *uamqp_session, char *host, char *destination, int settle_mode);
void uamqp_open_receiver(struct uamqp_message_receiver *uamqp_message_receiver);
char * uamqp_pull_last_message();
void uamqp_receiver_accept_last_message(struct uamqp_message_receiver *uamqp_message_receiver);
void uamqp_receiver_release_last_message(struct uamqp_message_receiver *uamqp_message_receiver);
void uamqp_receiver_reject_last_message(struct uamqp_message_receiver *uamqp_message_receiver, char* error_condition, char* error_description);
void uamqp_close_receiver(struct uamqp_message_receiver *uamqp_message_receiver);

