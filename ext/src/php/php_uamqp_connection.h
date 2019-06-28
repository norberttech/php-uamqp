#include <azure_uamqp_c/uamqp.h>

#define PHP_UAMQP_CONNECTION_CLASS "Connection"

typedef struct _uaqmp_connection_object_properties {
    zend_string *host;
    zend_long *port;
    zend_bool *useTLS;
    zend_string *policyName;
    zend_string *policyKey;
    bool isConnected;
} uaqmp_connection_object_properties;

typedef struct _uamqp_connection_object {
    uaqmp_connection_object_properties properties;
    CONNECTION_HANDLE uamqpConnection;
    SESSION_HANDLE uamqpConnectionSession;
    zend_object zendObject;
} uamqp_connection_object;

PHP_MINIT_FUNCTION(uamqp_connection);
