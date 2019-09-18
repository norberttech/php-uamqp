#include "php_uamqp.h"
#include "../uamqp/uamqp.h"

PHP_MINIT_FUNCTION(uamqp_connection);

zend_class_entry *php_uamqp_connection_ce;

typedef struct _uaqmp_connection_object_properties {
    char *host;
    long port;
    char *policy_name;
    char *policy_key;
} php_uaqmp_connection_object_properties;

typedef struct _uamqp_connection_object {
    php_uaqmp_connection_object_properties properties;
    struct uamqp uamqp_connection;
    struct uamqp_session uamqp_session;
    zend_object connection_zend_object;
} php_uamqp_connection_object;

static inline php_uamqp_connection_object *php_uamqp_connection_fetch_object(zend_object *obj) {
    return (php_uamqp_connection_object *)((char*)(obj) - XtOffsetOf(php_uamqp_connection_object, connection_zend_object));
}