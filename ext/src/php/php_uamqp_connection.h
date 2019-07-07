#include "php_uamqp.h"
#include "../uamqp/uamqp_message_sender.h"

PHP_MINIT_FUNCTION(uamqp_connection);

zend_class_entry *php_uamqp_connection_ce(void);

typedef struct _uaqmp_connection_object_properties {
    zend_string *host;
    zend_long port;
    zend_bool useTLS;
    zend_string *policyName;
    zend_string *policyKey;
} uaqmp_connection_object_properties;

typedef struct _uamqp_connection_object {
    uaqmp_connection_object_properties properties;
    struct uamqp_connection uamqp_connection;
    struct uamqp_session uamqp_session;
    zend_object zendObject;
} uamqp_connection_object;


#define UAMQP_CONNECTION_OBJECT(obj) \
    (uamqp_connection_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

static inline uamqp_connection_object *php_uamqp_connection_fetch_object(zend_object *obj) {
    return (uamqp_connection_object *)((char*)(obj) - XtOffsetOf(uamqp_connection_object, zendObject));
}