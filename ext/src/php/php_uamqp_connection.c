#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdbool.h>
#include "../../php_uamqp.h"
#include "../uamqp/uamqp_connection.h"
#include "php_uamqp_connection.h"

zend_class_entry *uamqp_connection_class_entry;
#define this_ce uamqp_connection_class_entry
#define METHOD(name) PHP_METHOD(UAMQPConnection, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPConnection, name, arginfo, visibility)

#define UAMQP_CONNECTION_OBJECT(obj) \
    (uamqp_connection_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

zend_object_handlers uamqp_connection_object_handlers;

METHOD(__construct)
{
    zend_string *host, *policyName, *policyKey;
    zend_long *port;
    zend_bool *useTLS;

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

    RETVAL_LONG(object->properties.port);
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

    RETURN_BOOL(object->properties.isConnected);
}

METHOD(connect)
{
    zend_parse_parameters_none();

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    if (object->properties.isConnected) {
        php_printf("already connected");
        return ;
    }

    object->uamqpConnection = create_uamqp_connection(ZSTR_VAL(object->properties.host), object->properties.port, ZSTR_VAL(object->properties.policyName), ZSTR_VAL(object->properties.policyKey));
    object->uamqpConnectionSession = create_uamqp_session(object->uamqpConnection);
    MESSAGE_SENDER_HANDLE sender = create_message_sender(object->uamqpConnectionSession, ZSTR_VAL(object->properties.host), "testname");
    MESSAGE_HANDLE message = create_message("test message");
    send_message(object->uamqpConnection, sender, message);

    object->properties.isConnected = true;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_connection_class_functions[] = {
    ME(__construct, arginfo_void, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(host, arginfo_void, ZEND_ACC_PUBLIC)
    ME(port, arginfo_void, ZEND_ACC_PUBLIC)
    ME(useTLS, arginfo_void, ZEND_ACC_PUBLIC)
    ME(policyName, arginfo_void, ZEND_ACC_PUBLIC)
    ME(policyKey, arginfo_void, ZEND_ACC_PUBLIC)
    ME(isConnected, arginfo_void, ZEND_ACC_PUBLIC)
    ME(connect, arginfo_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void uamqp_connection_init(uamqp_connection_object *object)
{
    object->properties.isConnected = false;
}

zend_object *uamqp_create_object_handler(zend_class_entry *ce)
{
    uamqp_connection_object *connection = ecalloc(1, sizeof(uamqp_connection_object) + zend_object_properties_size(ce));

    uamqp_connection_init( connection );
    zend_object_std_init(&connection->zendObject, ce);
    object_properties_init(&connection->zendObject, ce);
    connection->zendObject.handlers = &uamqp_connection_object_handlers;

    return &connection->zendObject;
}

PHP_MINIT_FUNCTION(uamqp_connection) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_CONNECTION_CLASS, uamqp_connection_class_functions);

    ce.create_object = uamqp_create_object_handler;

    this_ce = zend_register_internal_class(&ce);

    memcpy(&uamqp_connection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_connection_object_handlers.offset = XtOffsetOf(uamqp_connection_object, zendObject);
    // todo implement free_obj

    return SUCCESS;
}
