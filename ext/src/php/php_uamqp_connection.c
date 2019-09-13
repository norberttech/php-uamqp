#include <php.h>
#include <stdbool.h>
#include <azure_uamqp_c/uamqp.h>
#include "php_uamqp_connection.h"

#define PHP_UAMQP_CONNECTION_CLASS "Connection"

#define METHOD(name) PHP_METHOD(UAMQPConnection, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPConnection, name, arginfo, visibility)

zend_class_entry *uamqp_connection_ce;
zend_object_handlers uamqp_connection_object_handlers;

zend_class_entry *php_uamqp_connection_ce(void)
{
    return uamqp_connection_ce;
}

METHOD(__construct)
{
    zend_string *host, *policyName, *policyKey;
    zend_long port;

    uamqp_connection_object *object;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 4, 4)
        Z_PARAM_STR_EX(host, 1, 0)
        Z_PARAM_LONG(port)
        Z_PARAM_STR_EX(policyName, 1, 0)
        Z_PARAM_STR_EX(policyKey, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_CONNECTION_OBJECT(getThis());

    object->properties.host = zend_string_copy(host);
    object->properties.port = port;
    object->properties.policyName = zend_string_copy(policyName);
    object->properties.policyKey = zend_string_copy(policyKey);

    object->uamqp_connection = create_uamqp_connection(ZSTR_VAL(object->properties.host), object->properties.port, ZSTR_VAL(object->properties.policyName), ZSTR_VAL(object->properties.policyKey));
    object->uamqp_session = create_uamqp_session(object->uamqp_connection);
}

METHOD(enableDebugMode)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    set_uamqp_connection_debug_mode(object->uamqp_connection, 1);
}

METHOD(disableDebugMode)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    set_uamqp_connection_debug_mode(object->uamqp_connection, 0);
}

METHOD(host)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    RETVAL_STR(object->properties.host);
}


METHOD(port)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    RETVAL_LONG((int) object->properties.port);
}


METHOD(policyName)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    RETVAL_STR(object->properties.policyName);
}

METHOD(policyKey)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    RETVAL_STR(object->properties.policyKey);
}

ZEND_BEGIN_ARG_INFO_EX(connection_construct_arginfo, 0, 0, 4)
    ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, policyName, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, policyKey, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(connection_void_string_arginfo, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(connection_void_long_arginfo, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(connection_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_connection_class_functions[] = {
    ME(__construct, connection_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(enableDebugMode, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(disableDebugMode, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(host, connection_void_string_arginfo, ZEND_ACC_PUBLIC)
    ME(port, connection_void_long_arginfo, ZEND_ACC_PUBLIC)
    ME(policyName, connection_void_string_arginfo, ZEND_ACC_PUBLIC)
    ME(policyKey, connection_void_string_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_object *uamqp_object_handler_create(zend_class_entry *ce)
{
    uamqp_connection_object *connection = ecalloc(1, sizeof(uamqp_connection_object) + zend_object_properties_size(ce));

    zend_object_std_init(&connection->zendObject, ce);
    object_properties_init(&connection->zendObject, ce);
    connection->zendObject.handlers = &uamqp_connection_object_handlers;

    return &connection->zendObject;
}

void uamqp_connection_object_handler_free(zend_object *object)
{
    uamqp_connection_object *connection = php_uamqp_connection_fetch_object(object);

    destroy_connection(&connection->uamqp_connection, &connection->uamqp_session);
    zend_string_release(connection->properties.host);
    zend_string_release(connection->properties.policyKey);
    zend_string_release(connection->properties.policyName);
    zend_object_std_dtor(&connection->zendObject);
}

PHP_MINIT_FUNCTION(uamqp_connection) {
    zend_class_entry connection_ce;

    INIT_NS_CLASS_ENTRY(connection_ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_CONNECTION_CLASS, uamqp_connection_class_functions);

    connection_ce.create_object = uamqp_object_handler_create;

    uamqp_connection_ce = zend_register_internal_class(&connection_ce);

    memcpy(&uamqp_connection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_connection_object_handlers.offset = XtOffsetOf(uamqp_connection_object, zendObject);
    uamqp_connection_object_handlers.free_obj = uamqp_connection_object_handler_free;

    return SUCCESS;
}
