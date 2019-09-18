#include <php.h>
#include <ext/standard/php_var.h>
#include "php_uamqp_destination.h"
#include "php_uamqp_exception.h"

zend_object_handlers uamqp_destination_object_handlers;

#define METHOD(name) PHP_METHOD(UAMQPDestination, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPDestination, name, arginfo, visibility)

METHOD(__construct)
{
    char *destination;
    size_t destination_length;
    php_uamqp_destination_object *destination_object;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
        Z_PARAM_STRING_EX(destination, destination_length, 1, 1)
    ZEND_PARSE_PARAMETERS_END();

    destination_object = php_uamqp_destination_fetch_object(Z_OBJ_P(getThis()));

    if (destination_length > 64500) {
        php_uamqp_throw_exception("Destination can't be longer than 64500 characters.", 0);
    }

    destination_object->value = estrdup(destination);
}

METHOD(value)
{
    zend_parse_parameters_none();

    php_uamqp_destination_object *destination_object = php_uamqp_destination_fetch_object(Z_OBJ_P(getThis()));

    RETURN_STRING(destination_object->value);
}

ZEND_BEGIN_ARG_INFO_EX(destination_construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(destination_void_string_arginfo, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_destination_class_functions[] = {
    ME(__construct, destination_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(value, destination_void_string_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_object *uamqp_destination_handler_create(zend_class_entry *ce)
{
    php_uamqp_destination_object *destination = ecalloc(1, sizeof(php_uamqp_destination_object) + zend_object_properties_size(ce));

    zend_object_std_init(&destination->destination_zend_object, ce);
    object_properties_init(&destination->destination_zend_object, ce);
    destination->destination_zend_object.handlers = &uamqp_destination_object_handlers;

    return &destination->destination_zend_object;
}

void uamqp_destination_object_handler_free(zend_object *object)
{
    php_uamqp_destination_object *destination = php_uamqp_destination_fetch_object(object);

    efree(destination->value);
    zend_object_std_dtor(&destination->destination_zend_object);
}

static HashTable* uamqp_destination_object_debug_info(zval *obj, int *is_temp)
{
    zval ary;
    php_uamqp_destination_object *destination = php_uamqp_destination_fetch_object(Z_OBJ_P(obj));

    *is_temp = 1;

    array_init(&ary);

    if (!destination) {
        return Z_ARRVAL(ary);
    }

    add_assoc_string(&ary, "destination", destination->value);

    return Z_ARRVAL(ary);
}

PHP_MINIT_FUNCTION(uamqp_destination) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_DESTINATION_CLASS, uamqp_destination_class_functions);

    ce.create_object = uamqp_destination_handler_create;

    php_uamqp_destination_ce = zend_register_internal_class(&ce);

    memcpy(&uamqp_destination_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_destination_object_handlers.offset = XtOffsetOf(php_uamqp_destination_object, destination_zend_object);
    uamqp_destination_object_handlers.get_debug_info = uamqp_destination_object_debug_info;
    uamqp_destination_object_handlers.free_obj = uamqp_destination_object_handler_free;

    return SUCCESS;
}

