#include "php_uamqp.h"
#include "php_uamqp_connection.h"

#define PHP_UAMQP_CONSUMER_CLASS "Consumer"

PHP_MINIT_FUNCTION(uamqp_consumer);

zend_class_entry *php_uamqp_consumer_ce;

typedef struct _uamqp_consumer_object {
    php_uamqp_connection_object *uamqp_connection;
    long settle_mode;
    zend_object zendObject;
} uamqp_consumer_object;

static inline uamqp_consumer_object *php_uamqp_consumer_fetch_object(zend_object *obj) {
    return (uamqp_consumer_object *)((char*)(obj) - XtOffsetOf(uamqp_consumer_object, zendObject));
}