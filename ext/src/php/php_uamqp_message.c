#include <php.h>
#include "../../php_uamqp.h"
#include "php_uamqp_message.h"

zend_class_entry *uamqp_message_ce;
zend_object_handlers uamqp_message_object_handlers;

#define this_ce uamqp_message_ce
#define METHOD(name) PHP_METHOD(UAMQPMessage, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPMessage, name, arginfo, visibility)

zend_class_entry *php_uamqp_message_ce(void)
{
    return uamqp_message_ce;
}

METHOD(__construct)
{
    zend_string *payload;
    zend_string *destination;
    uamqp_message_object *object;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR_EX(payload, 1, 0)
        Z_PARAM_STR_EX(destination, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_MESSAGE_OBJECT(getThis());

    object->payload = payload;
    object->destination = destination;
}

METHOD(payload)
{
    zend_parse_parameters_none();

    uamqp_message_object *message = UAMQP_MESSAGE_OBJECT(getThis());

    RETVAL_STR(message->payload);
}

METHOD(destination)
{
    zend_parse_parameters_none();

    uamqp_message_object *message = UAMQP_MESSAGE_OBJECT(getThis());

    RETVAL_STR(message->destination);
}

ZEND_BEGIN_ARG_INFO_EX(message_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_message_class_functions[] = {
    ME(__construct, message_void_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(payload, message_void_arginfo, ZEND_ACC_PUBLIC)
    ME(destination, message_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_object *uamqp_message_handler_create(zend_class_entry *ce)
{
    uamqp_message_object *message = ecalloc(1, sizeof(uamqp_message_object) + zend_object_properties_size(ce));

    zend_object_std_init(&message->zendObject, ce);
    object_properties_init(&message->zendObject, ce);
    message->zendObject.handlers = &uamqp_message_object_handlers;

    return &message->zendObject;
}

void uamqp_message_object_handler_free(zend_object *object)
{
    uamqp_message_object *message = php_uamqp_message_fetch_object(object);

    message->payload = NULL;
    message->destination = NULL;
    zend_object_std_dtor(&message->zendObject);
}

PHP_MINIT_FUNCTION(uamqp_message) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_MESSAGE_CLASS, uamqp_message_class_functions);

    ce.create_object = uamqp_message_handler_create;

    this_ce = zend_register_internal_class(&ce);

    memcpy(&uamqp_message_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_message_object_handlers.offset = XtOffsetOf(uamqp_message_object, zendObject);
    uamqp_message_object_handlers.free_obj = uamqp_message_object_handler_free;

    return SUCCESS;
}

