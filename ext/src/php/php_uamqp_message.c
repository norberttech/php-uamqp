#include <php.h>
#include "php_uamqp_message.h"
#include "php_uamqp_exception.h"

zend_object_handlers uamqp_message_object_handlers;

#define METHOD(name) PHP_METHOD(UAMQPMessage, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPMessage, name, arginfo, visibility)

METHOD(__construct)
{
    char *payload;
    size_t payload_length;
    php_uamqp_message_object *message_object;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
        Z_PARAM_STRING_EX(payload, payload_length, 1, 1)
    ZEND_PARSE_PARAMETERS_END();

    message_object = php_uamqp_message_fetch_object(Z_OBJ_P(getThis()));

    if (payload_length > 64500) {
        php_uamqp_throw_exception("Message payload can't be longer than 64500 characters.", 0);
    }

    message_object->payload = estrdup(payload);
}

METHOD(payload)
{
    zend_parse_parameters_none();

    php_uamqp_message_object *message = php_uamqp_message_fetch_object(Z_OBJ_P(getThis()));

    RETURN_STRING(message->payload);
}

ZEND_BEGIN_ARG_INFO_EX(message_construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, payload, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(message_void_string_arginfo, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_message_class_functions[] = {
    ME(__construct, message_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(payload, message_void_string_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_object *uamqp_message_handler_create(zend_class_entry *ce)
{
    php_uamqp_message_object *message = ecalloc(1, sizeof(php_uamqp_message_object) + zend_object_properties_size(ce));

    zend_object_std_init(&message->message_zend_object, ce);
    object_properties_init(&message->message_zend_object, ce);
    message->message_zend_object.handlers = &uamqp_message_object_handlers;

    return &message->message_zend_object;
}

void uamqp_message_object_handler_free(zend_object *object)
{
    php_uamqp_message_object *message = php_uamqp_message_fetch_object(object);

    efree(message->payload);
    zend_object_std_dtor(&message->message_zend_object);
}

static HashTable* uamqp_message_object_debug_info(zval *obj, int *is_temp)
{
    zval ary;
    php_uamqp_message_object *message = php_uamqp_message_fetch_object(Z_OBJ_P(obj));

    *is_temp = 1;

    array_init(&ary);

    if (!message) {
        return Z_ARRVAL(ary);
    }

    add_assoc_string(&ary, "payload", message->payload);

    return Z_ARRVAL(ary);
}

PHP_MINIT_FUNCTION(uamqp_message) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_MESSAGE_CLASS, uamqp_message_class_functions);

    ce.create_object = uamqp_message_handler_create;

    php_uamqp_message_ce = zend_register_internal_class(&ce);

    memcpy(&uamqp_message_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_message_object_handlers.offset = XtOffsetOf(php_uamqp_message_object, message_zend_object);
    uamqp_message_object_handlers.get_debug_info = uamqp_message_object_debug_info;
    uamqp_message_object_handlers.free_obj = uamqp_message_object_handler_free;

    return SUCCESS;
}

