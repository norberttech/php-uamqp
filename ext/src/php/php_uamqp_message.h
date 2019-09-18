#include "php_uamqp.h"

#define PHP_UAMQP_MESSAGE_CLASS "Message"

PHP_MINIT_FUNCTION(uamqp_message);

zend_class_entry *php_uamqp_message_ce;

typedef struct _php_uamqp_message_object {
    char *payload;
    zend_object zendObject;
} php_uamqp_message_object;

static inline php_uamqp_message_object *php_uamqp_message_fetch_object(zend_object *obj) {
    return (php_uamqp_message_object *)((char*)(obj) - XtOffsetOf(php_uamqp_message_object, zendObject));
}