#include "php_uamqp.h"
#include "php_uamqp_connection.h"

#define PHP_UAMQP_PRODUCER_CLASS "Producer"

PHP_MINIT_FUNCTION(uamqp_producer);

zend_class_entry *php_uamqp_producer_ce;

typedef struct _uamqp_producer_object {
    uamqp_connection_object *uamqp_connection;
    zend_object zendObject;
} uamqp_producer_object;

static inline uamqp_producer_object *php_uamqp_producer_fetch_object(zend_object *obj) {
    return (uamqp_producer_object *)((char*)(obj) - XtOffsetOf(uamqp_producer_object, zendObject));
}