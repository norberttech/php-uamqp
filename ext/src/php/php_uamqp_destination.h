#include "php_uamqp.h"

#define PHP_UAMQP_DESTINATION_CLASS "Destination"

PHP_MINIT_FUNCTION(uamqp_destination);

zend_class_entry *php_uamqp_destination_ce;

typedef struct _uamqp_destination_object {
    char *value;
    zend_object zendObject;
} php_uamqp_destination_object;

static inline php_uamqp_destination_object *php_uamqp_destination_fetch_object(zend_object *obj) {
    return (php_uamqp_destination_object *)((char*)(obj) - XtOffsetOf(php_uamqp_destination_object, zendObject));
}