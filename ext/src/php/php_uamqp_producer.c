#include <php.h>
#include <ext/standard/php_var.h>
#include "php_uamqp_producer.h"
#include "php_uamqp_connection.h"
#include "php_uamqp_destination.h"
#include "php_uamqp_message.h"

#define PHP_UAMQP_PRODUCER_CLASS "Producer"

#define METHOD(name) PHP_METHOD(UAMQPProducer, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPProducer, name, arginfo, visibility)

#define UAMQP_PRODUCER_OBJECT(obj) \
    (uamqp_producer_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

zend_class_entry *uamqp_producer_ce;
zend_object_handlers uamqp_producer_object_handlers;

typedef struct _uamqp_producer_object {
    uamqp_connection_object *uamqp_connection;
    zend_object zendObject;
} uamqp_producer_object;

static inline uamqp_producer_object *php_uamqp_producer_fetch_object(zend_object *obj) {
    return (uamqp_producer_object *)((char*)(obj) - XtOffsetOf(uamqp_producer_object, zendObject));
}

METHOD(__construct)
{
    zval *connectiona_argument;
    uamqp_connection_object *connection;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1);
        Z_PARAM_OBJECT_OF_CLASS_EX(connectiona_argument, php_uamqp_connection_ce(), 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    uamqp_producer_object *object = UAMQP_PRODUCER_OBJECT(getThis());

    object->uamqp_connection = UAMQP_CONNECTION_OBJECT(connectiona_argument);
}

METHOD(sendMessage)
{
    zval *message_argument;
    zval *destination_argument;
    uamqp_message_object *message;
    uamqp_destination_object *destination;
    uamqp_producer_object *object;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(message_argument, php_uamqp_message_ce(), 1, 0);
        Z_PARAM_OBJECT_OF_CLASS_EX(destination_argument, php_uamqp_destination_ce(), 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_PRODUCER_OBJECT(getThis());
    message = UAMQP_MESSAGE_OBJECT(message_argument);
    destination = UAMQP_DESTINATION_OBJECT(destination_argument);

    send_message(
        object->uamqp_connection->uamqp_connection,
        create_message_sender(object->uamqp_connection->uamqp_session, ZSTR_VAL(object->uamqp_connection->properties.host), ZSTR_VAL(destination->value)),
        create_message(message->payload)
    );
}

ZEND_BEGIN_ARG_INFO_EX(producer_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, message, UAMQP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(producer_sendMessage_arginfo, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, message, UAMQP\\Message, 0)
    ZEND_ARG_OBJ_INFO(0, destination, UAMQP\\Destination, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_producer_class_functions[] = {
    ME(__construct, producer_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(sendMessage, producer_sendMessage_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_object *uamqp_producer_handler_create(zend_class_entry *ce)
{
    uamqp_producer_object *producer = ecalloc(1, sizeof(uamqp_producer_object) + zend_object_properties_size(ce));

    zend_object_std_init(&producer->zendObject, ce);
    object_properties_init(&producer->zendObject, ce);
    producer->zendObject.handlers = &uamqp_producer_object_handlers;

    return &producer->zendObject;
}

void uamqp_producer_object_handler_free(zend_object *object)
{
    uamqp_producer_object *producer = php_uamqp_producer_fetch_object(object);

    producer->uamqp_connection = NULL;
    zend_object_std_dtor(&producer->zendObject);
}

PHP_MINIT_FUNCTION(uamqp_producer) {
    zend_class_entry ce_producer;

    INIT_NS_CLASS_ENTRY(ce_producer, PHP_UAMQP_NAMESPACE, PHP_UAMQP_PRODUCER_CLASS, uamqp_producer_class_functions);

    ce_producer.create_object = uamqp_producer_handler_create;

    uamqp_producer_ce = zend_register_internal_class(&ce_producer);

    memcpy(&uamqp_producer_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_producer_object_handlers.offset = XtOffsetOf(uamqp_producer_object, zendObject);
    uamqp_producer_object_handlers.free_obj = uamqp_producer_object_handler_free;

    return SUCCESS;
}

