#include "php_uamqp.h"

#define PHP_UAMQP_MESSAGE_CLASS "Message"

PHP_MINIT_FUNCTION(uamqp_message);

#define UAMQP_MESSAGE_OBJECT(obj) \
    (uamqp_message_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

zend_class_entry *php_uamqp_message_ce(void);
zend_object *uamqp_message_handler_create(zend_class_entry *ce);

typedef struct _uamqp_message_object {
    zend_string *payload;
    zend_object zendObject;
} uamqp_message_object;

static inline uamqp_message_object *php_uamqp_message_fetch_object(zend_object *obj) {
    return (uamqp_message_object *)((char*)(obj) - XtOffsetOf(uamqp_message_object, zendObject));
}