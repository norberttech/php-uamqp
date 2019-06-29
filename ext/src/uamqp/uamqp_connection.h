#include <azure_uamqp_c/uamqp.h>

struct uamqp_connection {
    CONNECTION_HANDLE connection;
    XIO_HANDLE tls_io;
    XIO_HANDLE sasl_io;
    SASL_MECHANISM_HANDLE sasl_mechanism_handle;
};

struct uamqp_session {
    SESSION_HANDLE session;
};

struct uamqp_connection create_uamqp_connection(char *host, int port, char *policyName, char *policyKey);
struct uamqp_session create_uamqp_session(struct uamqp_connection connection);

void destroy_connection(struct uamqp_connection *uamqp_connection, struct uamqp_session *uamqp_session);