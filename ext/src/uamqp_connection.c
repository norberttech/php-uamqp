#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/php_var.h>
#include <stdbool.h>
#include "../php_uamqp.h"
#include "parameters.h"
#include "properties.h"
#include "uamqp_connection.h"
#include "azure_c_shared_utility/platform.h"

zend_class_entry *uamqp_connection_class_entry;
#define this_ce uamqp_connection_class_entry
#define METHOD(name) PHP_METHOD(UAMQPConnection, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPConnection, name, arginfo, visibility)

#define UAMQP_CONNECTION_OBJECT(obj) \
    (uamqp_connection_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

typedef struct _uamqp_connection {
    bool connected;
} uamqp_connection;

typedef struct _uamqp_connection_object {
    uamqp_connection connection;
    zend_object zendObject;
} uamqp_connection_object;

zend_object_handlers uamqp_connection_object_handlers;

METHOD(isConnected)
{
    PARSE_NONE;

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    RETURN_BOOL(object->connection.connected);
}

METHOD(connect)
{
    PARSE_NONE;

    uamqp_connection_object *object = UAMQP_CONNECTION_OBJECT(getThis());

    if (object->connection.connected == true) {
        php_printf("already connected");
        return ;
    }

    if (platform_init() != 0) {
        // not sure what to do when
    }

    object->connection.connected = true;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_connection_class_functions[] = {
    ME(isConnected, arginfo_void, ZEND_ACC_PUBLIC)
    ME(connect, arginfo_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void uamqp_connection_init(uamqp_connection *connection)
{
    if(!connection)
        return;

    connection->connected = false;
}

zend_object *uamqp_create_object_handler(zend_class_entry *ce)
{
    uamqp_connection_object *connection = ecalloc(1, sizeof(uamqp_connection_object) + zend_object_properties_size(ce));

    uamqp_connection_init( &connection->connection );
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
