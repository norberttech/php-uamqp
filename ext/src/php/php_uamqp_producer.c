#include <php.h>
#include <ext/standard/php_var.h>
#include "php_uamqp_producer.h"
#include "php_uamqp_destination.h"
#include "php_uamqp_message.h"

#define METHOD(name) PHP_METHOD(UAMQPProducer, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPProducer, name, arginfo, visibility)

zend_object_handlers uamqp_producer_object_handlers;

METHOD(__construct)
{
    zval *connection_object_argument;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1);
        Z_PARAM_OBJECT_OF_CLASS_EX(connection_object_argument, php_uamqp_connection_ce, 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    uamqp_producer_object *object = php_uamqp_producer_fetch_object(Z_OBJ_P(getThis()));

    object->uamqp_connection = php_uamqp_connection_fetch_object(Z_OBJ_P(connection_object_argument));
}

METHOD(sendMessage)
{
    zval *message_argument;
    zval *destination_argument;
    php_uamqp_message_object *message;
    php_uamqp_destination_object *destination;
    uamqp_producer_object *producer;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(message_argument, php_uamqp_message_ce, 1, 0);
        Z_PARAM_OBJECT_OF_CLASS_EX(destination_argument, php_uamqp_destination_ce, 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    producer = php_uamqp_producer_fetch_object(Z_OBJ_P(getThis()));
    message = php_uamqp_message_fetch_object(Z_OBJ_P(message_argument));
    destination = php_uamqp_destination_fetch_object(Z_OBJ_P(destination_argument));

    send_message(
        producer->uamqp_connection->uamqp_connection,
        create_message_sender(producer->uamqp_connection->uamqp_session, producer->uamqp_connection->properties.host, destination->value),
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

    zend_object_std_init(&producer->producer_zend_object, ce);
    object_properties_init(&producer->producer_zend_object, ce);
    producer->producer_zend_object.handlers = &uamqp_producer_object_handlers;

    return &producer->producer_zend_object;
}

void uamqp_producer_object_handler_free(zend_object *object)
{
    uamqp_producer_object *producer = php_uamqp_producer_fetch_object(object);

    producer->uamqp_connection = NULL;
    zend_object_std_dtor(&producer->producer_zend_object);
}

PHP_MINIT_FUNCTION(uamqp_producer) {
    zend_class_entry ce_producer;

    INIT_NS_CLASS_ENTRY(ce_producer, PHP_UAMQP_NAMESPACE, PHP_UAMQP_PRODUCER_CLASS, uamqp_producer_class_functions);

    ce_producer.create_object = uamqp_producer_handler_create;

    php_uamqp_producer_ce = zend_register_internal_class(&ce_producer);

    memcpy(&uamqp_producer_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_producer_object_handlers.offset = XtOffsetOf(uamqp_producer_object, producer_zend_object);
    uamqp_producer_object_handlers.free_obj = uamqp_producer_object_handler_free;

    return SUCCESS;
}

