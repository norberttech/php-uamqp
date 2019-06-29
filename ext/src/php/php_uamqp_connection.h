#include <azure_uamqp_c/uamqp.h>
#include "../uamqp/uamqp_connection.h"

#define PHP_UAMQP_CONNECTION_CLASS "Connection"

typedef struct _uaqmp_connection_object_properties {
    zend_string *host;
    zend_long port;
    zend_bool useTLS;
    zend_string *policyName;
    zend_string *policyKey;
    bool isConnected;
} uaqmp_connection_object_properties;

typedef struct _uamqp_connection_object {
    uaqmp_connection_object_properties properties;
    struct uamqp_connection uamqp_connection;
    struct uamqp_session uamqp_session;
    zend_object zendObject;
} uamqp_connection_object;

PHP_MINIT_FUNCTION(uamqp_connection);
