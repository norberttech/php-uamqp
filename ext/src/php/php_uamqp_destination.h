#include "php_uamqp.h"

#define PHP_UAMQP_DESTINATION_CLASS "Destination"

PHP_MINIT_FUNCTION(uamqp_destination);

#define UAMQP_DESTINATION_OBJECT(obj) \
    (uamqp_destination_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

zend_class_entry *php_uamqp_destination_ce(void);
zend_object *uamqp_destination_handler_create(zend_class_entry *ce);

typedef struct _uamqp_destination_object {
    zend_string *value;
    zend_object zendObject;
} uamqp_destination_object;

static inline uamqp_destination_object *php_uamqp_destination_fetch_object(zend_object *obj) {
    return (uamqp_destination_object *)((char*)(obj) - XtOffsetOf(uamqp_destination_object, zendObject));
}