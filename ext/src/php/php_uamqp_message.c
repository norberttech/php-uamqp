#include <php.h>
#include <Zend/zend_exceptions.h>
#include "../../php_uamqp.h"
#include "php_uamqp_message.h"
#include "php_uamqp_exception.h"

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
    uamqp_message_object *object;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
        Z_PARAM_STR_EX(payload, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_MESSAGE_OBJECT(getThis());

    if (payload->len > 64500) {
        zend_throw_exception(php_uamqp_exception_ce(), "Message payload can't be longer than 64500 characters.", 0);
    }

    object->payload = zend_string_copy(payload);
}

METHOD(payload)
{
    zend_parse_parameters_none();

    uamqp_message_object *message = UAMQP_MESSAGE_OBJECT(getThis());

    RETVAL_STR(message->payload);
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
    uamqp_message_object *message = ecalloc(1, sizeof(uamqp_message_object) + zend_object_properties_size(ce));

    zend_object_std_init(&message->zendObject, ce);
    object_properties_init(&message->zendObject, ce);
    message->zendObject.handlers = &uamqp_message_object_handlers;

    return &message->zendObject;
}

void uamqp_message_object_handler_free(zend_object *object)
{
    uamqp_message_object *message = php_uamqp_message_fetch_object(object);

    zend_string_release(message->payload);
    zend_object_std_dtor(&message->zendObject);
}

static HashTable* uamqp_message_object_debug_info(zval *obj, int *is_temp)
{
    uamqp_message_object *message = php_uamqp_message_fetch_object(Z_OBJ_P(obj));
    zval tmp;
    zend_string *payload_key;
    HashTable *debug_info;

    *is_temp = 1;

    ALLOC_HASHTABLE(debug_info);
    zend_hash_init(debug_info, 1, NULL, ZVAL_PTR_DTOR, 0);

    ZVAL_STR(&tmp, message->payload);
    payload_key = zend_string_init("payload", sizeof("payload") - 1, 0);
    zend_hash_update(debug_info, payload_key, &tmp);
    zend_string_release(payload_key);
    zval_dtor(&tmp);

    return debug_info;
}

PHP_MINIT_FUNCTION(uamqp_message) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_MESSAGE_CLASS, uamqp_message_class_functions);

    ce.create_object = uamqp_message_handler_create;

    this_ce = zend_register_internal_class(&ce);

    memcpy(&uamqp_message_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_message_object_handlers.offset = XtOffsetOf(uamqp_message_object, zendObject);
    uamqp_message_object_handlers.get_debug_info = uamqp_message_object_debug_info;
    uamqp_message_object_handlers.free_obj = uamqp_message_object_handler_free;

    return SUCCESS;
}

