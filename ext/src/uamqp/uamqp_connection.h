#include <azure_uamqp_c/uamqp.h>

CONNECTION_HANDLE create_uamqp_connection(char *host, long port, char *policyName, char *policyKey);
SESSION_HANDLE create_uamqp_session(CONNECTION_HANDLE connection);
MESSAGE_SENDER_HANDLE create_message_sender(SESSION_HANDLE session, char *host, char *destination);
MESSAGE_HANDLE create_message(char *content);
void send_message(CONNECTION_HANDLE connection, MESSAGE_SENDER_HANDLE message_sender, MESSAGE_HANDLE message);