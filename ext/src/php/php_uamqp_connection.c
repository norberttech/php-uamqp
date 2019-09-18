#include <php.h>
#include <azure_uamqp_c/uamqp.h>
#include "php_uamqp_connection.h"

#define PHP_UAMQP_CONNECTION_CLASS "Connection"

#define METHOD(name) PHP_METHOD(UAMQPConnection, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPConnection, name, arginfo, visibility)

zend_object_handlers uamqp_connection_object_handlers;

METHOD(__construct)
{
    char *host, *policy_name, *policy_key;
    size_t host_length, policy_name_length, policy_key_length;
    long port;

    php_uamqp_connection_object *connection_object;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 4, 4)
        Z_PARAM_STRING_EX(host, host_length, 1, 0)
        Z_PARAM_LONG(port)
        Z_PARAM_STRING_EX(policy_name, policy_name_length, 1, 0)
        Z_PARAM_STRING_EX(policy_key, policy_key_length, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    connection_object = php_uamqp_connection_fetch_object(Z_OBJ_P(getThis()));

    connection_object->properties.host = estrdup(host);
    connection_object->properties.port = port;
    connection_object->properties.policyName = estrdup(policy_name);
    connection_object->properties.policyKey = estrdup(policy_key);

    connection_object->uamqp_connection = create_uamqp_connection(connection_object->properties.host, (int) connection_object->properties.port, connection_object->properties.policyName, connection_object->properties.policyKey);
    connection_object->uamqp_session = create_uamqp_session(connection_object->uamqp_connection);
}

METHOD(enableDebugMode)
{
    zend_parse_parameters_none();

    php_uamqp_connection_object *object = php_uamqp_connection_fetch_object(Z_OBJ_P(getThis()));

    set_uamqp_connection_debug_mode(object->uamqp_connection, 1);
}

METHOD(disableDebugMode)
{
    zend_parse_parameters_none();

    php_uamqp_connection_object *object = php_uamqp_connection_fetch_object(Z_OBJ_P(getThis()));

    set_uamqp_connection_debug_mode(object->uamqp_connection, 0);
}

METHOD(host)
{
    zend_parse_parameters_none();

    php_uamqp_connection_object *object = php_uamqp_connection_fetch_object(Z_OBJ_P(getThis()));

    RETURN_STRING(object->properties.host);
}


METHOD(port)
{
    zend_parse_parameters_none();

    php_uamqp_connection_object *object = php_uamqp_connection_fetch_object(Z_OBJ_P(getThis()));

    RETVAL_LONG(object->properties.port);
}


METHOD(policyName)
{
    zend_parse_parameters_none();

    php_uamqp_connection_object *object = php_uamqp_connection_fetch_object(Z_OBJ_P(getThis()));

    RETURN_STRING(object->properties.policyName);
}

METHOD(policyKey)
{
    zend_parse_parameters_none();

    php_uamqp_connection_object *object = php_uamqp_connection_fetch_object(Z_OBJ_P(getThis()));

    RETURN_STRING(object->properties.policyKey);
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
    php_uamqp_connection_object *connection = ecalloc(1, sizeof(php_uamqp_connection_object) + zend_object_properties_size(ce));

    zend_object_std_init(&connection->zendObject, ce);
    object_properties_init(&connection->zendObject, ce);
    connection->zendObject.handlers = &uamqp_connection_object_handlers;

    return &connection->zendObject;
}

void uamqp_connection_object_handler_free(zend_object *object)
{
    php_uamqp_connection_object *connection = php_uamqp_connection_fetch_object(object);

    destroy_connection(&connection->uamqp_connection, &connection->uamqp_session);
    efree(connection->properties.host);
    efree(connection->properties.policyKey);
    efree(connection->properties.policyName);
    zend_object_std_dtor(&connection->zendObject);
}

PHP_MINIT_FUNCTION(uamqp_connection) {
    zend_class_entry connection_ce;

    INIT_NS_CLASS_ENTRY(connection_ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_CONNECTION_CLASS, uamqp_connection_class_functions);

    connection_ce.create_object = uamqp_object_handler_create;

    php_uamqp_connection_ce = zend_register_internal_class(&connection_ce);

    memcpy(&uamqp_connection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_connection_object_handlers.offset = XtOffsetOf(php_uamqp_connection_object, zendObject);
    uamqp_connection_object_handlers.free_obj = uamqp_connection_object_handler_free;

    return SUCCESS;
}
