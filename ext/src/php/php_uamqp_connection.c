#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdbool.h>
#include "php_uamqp_connection.h"
#include "../../php_uamqp.h"
#include "../uamqp/uamqp_message_sender.h"

zend_class_entry *uamqp_connection_class_entry;
zend_object_handlers uamqp_connection_object_handlers;

#define this_ce uamqp_connection_class_entry
#define METHOD(name) PHP_METHOD(UAMQPConnection, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPConnection, name, arginfo, visibility)

typedef struct _uaqmp_connection_object_properties {
    zend_string *host;
    zend_long port;
    zend_bool useTLS;
    zend_string *policyName;
    zend_string *policyKey;
    bool isConnected;
} uaqmp_connection_object_properties;

typedef struct _uamqp_connection_object {
    uaqmp_connection_object_properties properties;
    struct uamqp_connection uamqp_connection;
    struct uamqp_session uamqp_session;
    zend_object zendObject;
} uamqp_connection_object;

#define UAMQP_CONNECTION_OBJECT(obj) \
    (uamqp_connection_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

static inline uamqp_connection_object *php_uamqp_connection_fetch_object(zend_object *obj) {
    return (uamqp_connection_object *)((char*)(obj) - XtOffsetOf(uamqp_connection_object, zendObject));
}

METHOD(__construct)
{
    zend_string *host, *policyName, *policyKey;
    zend_long port;
    zend_bool useTLS;

    uamqp_connection_object *object;

    ZEND_PARSE_PARAMETERS_START(5, 5)
        Z_PARAM_STR(host)
        Z_PARAM_LONG(port)
        Z_PARAM_BOOL(useTLS)
        Z_PARAM_STR(policyName)
        Z_PARAM_STR(policyKey)
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_CONNECTION_OBJECT(getThis());

    object->properties.host = host;
    object->properties.port = port;
    object->properties.useTLS = useTLS;
    object->properties.policyName = policyName;
    object->properties.policyKey = policyKey;
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

METHOD(useTLS)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    RETVAL_BOOL(object->properties.useTLS);
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

METHOD(isConnected)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    RETURN_BOOL(object->uamqp_connection.connection != NULL);
}

METHOD(disconnect)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    if (object->uamqp_connection.connection == NULL) {
        // already dicconnected
        return ;
    }

    destroy_connection(&object->uamqp_connection, &object->uamqp_session);
}

METHOD(sendMessage)
{
    zend_string *message;
    zend_string *destination;
    uamqp_connection_object *object;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(message)
        Z_PARAM_STR(destination)
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_CONNECTION_OBJECT(getThis());

    if (object->uamqp_connection.connection == NULL) {
        object->uamqp_connection = create_uamqp_connection(ZSTR_VAL(object->properties.host), object->properties.port, ZSTR_VAL(object->properties.policyName), ZSTR_VAL(object->properties.policyKey));
        object->uamqp_session = create_uamqp_session(object->uamqp_connection);
    }

    send_message(
        object->uamqp_connection,
        create_message_sender(object->uamqp_session, ZSTR_VAL(object->properties.host), ZSTR_VAL(destination)),
        create_message(ZSTR_VAL(message))
    );
}

ZEND_BEGIN_ARG_INFO_EX(connection_construct_arginfo, 0, 0, 0)
    ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, useTLS, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, policyName, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, policyKey, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(connection_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_connection_class_functions[] = {
    ME(__construct, connection_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(host, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(port, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(useTLS, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(policyName, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(policyKey, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(isConnected, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(disconnect, connection_void_arginfo, ZEND_ACC_PUBLIC)
    ME(sendMessage, connection_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void uamqp_connection_init(uamqp_connection_object *object)
{
    object->properties.isConnected = false;
}

zend_object *uamqp_object_handler_create(zend_class_entry *ce)
{
    uamqp_connection_object *connection = ecalloc(1, sizeof(uamqp_connection_object) + zend_object_properties_size(ce));

    uamqp_connection_init( connection );
    zend_object_std_init(&connection->zendObject, ce);
    object_properties_init(&connection->zendObject, ce);
    connection->zendObject.handlers = &uamqp_connection_object_handlers;

    return &connection->zendObject;
}

void uamqp_connection_object_handler_free( zend_object *object )
{
    uamqp_connection_object *connection = php_uamqp_connection_fetch_object(object);

    if (connection->uamqp_connection.connection == NULL) {
        // already dicconnected
        return ;
    }

    destroy_connection(&connection->uamqp_connection, &connection->uamqp_session);

    zend_object_std_dtor(&connection->zendObject);
}

PHP_MINIT_FUNCTION(uamqp_connection) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_CONNECTION_CLASS, uamqp_connection_class_functions);

    ce.create_object = uamqp_object_handler_create;

    this_ce = zend_register_internal_class(&ce);

    memcpy(&uamqp_connection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_connection_object_handlers.offset = XtOffsetOf(uamqp_connection_object, zendObject);
    uamqp_connection_object_handlers.free_obj = uamqp_connection_object_handler_free;

    return SUCCESS;
}
