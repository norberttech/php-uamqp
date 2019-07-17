#include <php.h>
#include <ext/standard/php_var.h>
#include "../../php_uamqp.h"
#include "php_uamqp_destination.h"

zend_class_entry *uamqp_destination_ce;
zend_object_handlers uamqp_destination_object_handlers;

#define this_ce uamqp_destination_ce
#define METHOD(name) PHP_METHOD(UAMQPDestination, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPDestination, name, arginfo, visibility)

zend_class_entry *php_uamqp_destination_ce(void)
{
    return uamqp_destination_ce;
}

METHOD(__construct)
{
    zend_string *destination_value;
    uamqp_destination_object *object;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
        Z_PARAM_STR_EX(destination_value, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_DESTINATION_OBJECT(getThis());

    object->value = zend_string_copy(destination_value);
}

METHOD(value)
{
    zend_parse_parameters_none();

    uamqp_destination_object *destination = UAMQP_DESTINATION_OBJECT(getThis());

    RETVAL_STR(destination->value);
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
    uamqp_destination_object *destination = ecalloc(1, sizeof(uamqp_destination_object) + zend_object_properties_size(ce));

    zend_object_std_init(&destination->zendObject, ce);
    object_properties_init(&destination->zendObject, ce);
    destination->zendObject.handlers = &uamqp_destination_object_handlers;

    return &destination->zendObject;
}

void uamqp_destination_object_handler_free(zend_object *object)
{
    uamqp_destination_object *destination = php_uamqp_destination_fetch_object(object);

    destination->value = NULL;
    zend_object_std_dtor(&destination->zendObject);
}

static HashTable* uamqp_destination_object_debug_info(zval *obj, int *is_temp) /* {{{ */
{
    uamqp_destination_object *destination = php_uamqp_destination_fetch_object(Z_OBJ_P(obj));
    HashTable *props;
    zval tmp;
    HashTable *debug_info;

    *is_temp = 1;

    props = Z_OBJPROP_P(obj);

    ALLOC_HASHTABLE(debug_info);
    ZEND_INIT_SYMTABLE_EX(debug_info, zend_hash_num_elements(props) + 1, 0);
    zend_hash_copy(debug_info, props, (copy_ctor_func_t)zval_add_ref);

    ZVAL_STR(&tmp, destination->value);
    zend_hash_update(debug_info, zend_string_init("destination", sizeof("destination") - 1, 0), &tmp);
    zval_dtor(&tmp);

    return debug_info;
}

PHP_MINIT_FUNCTION(uamqp_destination) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_DESTINATION_CLASS, uamqp_destination_class_functions);

    ce.create_object = uamqp_destination_handler_create;

    this_ce = zend_register_internal_class(&ce);

    memcpy(&uamqp_destination_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_destination_object_handlers.offset = XtOffsetOf(uamqp_destination_object, zendObject);
    uamqp_destination_object_handlers.get_debug_info = uamqp_destination_object_debug_info;
    uamqp_destination_object_handlers.free_obj = uamqp_destination_object_handler_free;

    return SUCCESS;
}

