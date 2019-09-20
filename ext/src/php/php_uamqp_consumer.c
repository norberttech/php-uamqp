#include <php.h>
#include <string.h>
#include <Zend/zend_interfaces.h>
#include "php_uamqp.h"
#include "php_uamqp_message.h"
#include "php_uamqp_destination.h"
#include "php_uamqp_consumer.h"
#include "php_uamqp_exception.h"

#define METHOD(name) PHP_METHOD(UAMQPConsumer, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPConsumer, name, arginfo, visibility)

static const int RECEIVER_RECEIVE_AND_DELETE = 0;
static const int RECEIVER_PEAK_AND_LOCK = 1;

zend_object_handlers uamqp_consumer_object_handlers;

METHOD(__construct)
{
    zval *connection_object_argument;
    uamqp_consumer_object *consumer_object;
    long settle_mode_argument;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2);
        Z_PARAM_OBJECT_OF_CLASS_EX(connection_object_argument, php_uamqp_connection_ce, 1, 0);
        Z_PARAM_LONG(settle_mode_argument);
    ZEND_PARSE_PARAMETERS_END();

    consumer_object = php_uamqp_consumer_fetch_object(Z_OBJ_P(getThis()));

    consumer_object->uamqp_connection = php_uamqp_connection_fetch_object(Z_OBJ_P(connection_object_argument));
    consumer_object->settle_mode = settle_mode_argument;
}

METHOD(open)
{
    uamqp_consumer_object *consumer_object;
    php_uamqp_destination_object *destination;
    zval *destination_argument;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1);
            Z_PARAM_OBJECT_OF_CLASS_EX(destination_argument, php_uamqp_destination_ce, 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    consumer_object = php_uamqp_consumer_fetch_object(Z_OBJ_P(getThis()));
    destination = php_uamqp_destination_fetch_object(Z_OBJ_P(destination_argument));

    if (consumer_object->uamqp_receiver.state == RECEIVER_OPEN) {
        php_uamqp_throw_exception("Receiver already opened", 0);
    } else {
        consumer_object->uamqp_receiver = create_message_receiver(
            &consumer_object->uamqp_connection->uamqp_session,
            consumer_object->uamqp_connection->properties.host,
            destination->value,
            (int) consumer_object->settle_mode
        );

        uamqp_open_receiver(&consumer_object->uamqp_receiver);
    }
}

METHOD(receive)
{
    uamqp_consumer_object *consumer_object;
    char *last_message = NULL;

    consumer_object = php_uamqp_consumer_fetch_object(Z_OBJ_P(getThis()));

    if (consumer_object->uamqp_receiver.state != RECEIVER_OPEN) {
        php_uamqp_throw_exception("Receiver is not open.", 0);

        return ;
    }

    last_message = uamqp_pull_last_message();

    if (last_message) {
        php_uamqp_message_object *message;

        object_init_ex(return_value, php_uamqp_message_ce);

        message = php_uamqp_message_fetch_object(Z_OBJ_P(return_value));
        message->payload = estrdup(last_message);

        if (consumer_object->settle_mode == RECEIVER_RECEIVE_AND_DELETE) {
            uamqp_receiver_accept_last_message(&consumer_object->uamqp_receiver);
        }

    } else {
        uamqp_connection_work(consumer_object->uamqp_connection->uamqp_connection);

        RETURN_NULL();
    }
}

METHOD(accept)
{
    uamqp_consumer_object *consumer_object;
    char *last_message = NULL;

    consumer_object = php_uamqp_consumer_fetch_object(Z_OBJ_P(getThis()));

    if (consumer_object->uamqp_receiver.state != RECEIVER_OPEN) {
        php_uamqp_throw_exception("Receiver is not open.", 0);

        return ;
    }

    if (consumer_object->settle_mode != RECEIVER_PEAK_AND_LOCK) {
        php_uamqp_throw_exception("Accepting messages is available only in PEAK_AND_LOCK settle mode.", 0);
        return ;
    }

    last_message = uamqp_pull_last_message();

    if (last_message) {
        uamqp_receiver_accept_last_message(&consumer_object->uamqp_receiver);
    }
}

METHOD(release)
{
    uamqp_consumer_object *consumer_object;
    char *last_message = NULL;

    consumer_object = php_uamqp_consumer_fetch_object(Z_OBJ_P(getThis()));

    if (consumer_object->uamqp_receiver.state != RECEIVER_OPEN) {
        php_uamqp_throw_exception("Receiver is not open.", 0);

        return ;
    }

    if (consumer_object->settle_mode != RECEIVER_PEAK_AND_LOCK) {
        php_uamqp_throw_exception("Releasing messages is available only in PEAK_AND_LOCK settle mode.", 0);
        return ;
    }

    last_message = uamqp_pull_last_message();

    if (last_message) {
        uamqp_receiver_release_last_message(&consumer_object->uamqp_receiver);
    }
}

METHOD(reject)
{
    uamqp_consumer_object *consumer_object;
    char *error_condition = NULL, *error_description = NULL, *last_message = NULL;
    size_t error_condition_length, error_description_length;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
        Z_PARAM_STRING_EX(error_condition, error_condition_length, 1, 0)
        Z_PARAM_STRING_EX(error_description, error_description_length, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    consumer_object = php_uamqp_consumer_fetch_object(Z_OBJ_P(getThis()));

    if (consumer_object->uamqp_receiver.state != RECEIVER_OPEN) {
        php_uamqp_throw_exception("Receiver is not open.", 0);

        return ;
    }

    if (consumer_object->settle_mode != RECEIVER_PEAK_AND_LOCK) {
        php_uamqp_throw_exception("Rejecting messages is available only in PEAK_AND_LOCK settle mode.", 0);
        return ;
    }

    last_message = uamqp_pull_last_message();

    if (last_message) {
        uamqp_receiver_reject_last_message(&consumer_object->uamqp_receiver, error_condition, error_description);
    }
}

METHOD(close)
{
    uamqp_consumer_object *consumer_object;
    consumer_object = php_uamqp_consumer_fetch_object(Z_OBJ_P(getThis()));

    if (consumer_object->uamqp_receiver.state == RECEIVER_OPEN) {
        uamqp_close_receiver(&consumer_object->uamqp_receiver);
    }
}

ZEND_BEGIN_ARG_INFO_EX(consumer_construct_arginfo, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, message, UAMQP\\Connection, 0)
    ZEND_ARG_TYPE_INFO(0, settleMode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(consumer_open_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, destination, UAMQP\\Destination, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(consumer_reject_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, errorCondition, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, errorDescription, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(consumer_no_args_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_consumer_class_functions[] = {
    ME(__construct, consumer_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(open, consumer_open_arginfo, ZEND_ACC_PUBLIC)
    ME(receive, consumer_no_args_arginfo, ZEND_ACC_PUBLIC)
    ME(accept, consumer_no_args_arginfo, ZEND_ACC_PUBLIC)
    ME(release, consumer_no_args_arginfo, ZEND_ACC_PUBLIC)
    ME(reject, consumer_reject_arginfo, ZEND_ACC_PUBLIC)
    ME(close, consumer_no_args_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_object *uamqp_consumer_handler_create(zend_class_entry *ce)
{
    uamqp_consumer_object *consumer = ecalloc(1, sizeof(uamqp_consumer_object) + zend_object_properties_size(ce));

    zend_object_std_init(&consumer->std, ce);
    object_properties_init(&consumer->std, ce);
    consumer->std.handlers = &uamqp_consumer_object_handlers;

    return &consumer->std;
}

void uamqp_consumer_object_handler_free(zend_object *object)
{
    uamqp_consumer_object *consumer_object = php_uamqp_consumer_fetch_object(object);

    if (consumer_object->uamqp_receiver.state == RECEIVER_OPEN) {
        uamqp_close_receiver(&consumer_object->uamqp_receiver);
    }

    consumer_object->uamqp_connection = NULL;
    zend_object_std_dtor(&consumer_object->std);
}

PHP_MINIT_FUNCTION(uamqp_consumer) {
    zend_class_entry ce_consumer;

    INIT_NS_CLASS_ENTRY(ce_consumer, PHP_UAMQP_NAMESPACE, PHP_UAMQP_CONSUMER_CLASS, uamqp_consumer_class_functions);

    ce_consumer.create_object = uamqp_consumer_handler_create;

    php_uamqp_consumer_ce = zend_register_internal_class(&ce_consumer);

    zend_declare_class_constant_long(php_uamqp_consumer_ce, ZEND_STRL("RECEIVE_AND_DELETE"), RECEIVER_RECEIVE_AND_DELETE);
    zend_declare_class_constant_long(php_uamqp_consumer_ce, ZEND_STRL("PEAK_AND_LOCK"), RECEIVER_PEAK_AND_LOCK);

    memcpy(&uamqp_consumer_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_consumer_object_handlers.offset = XtOffsetOf(uamqp_consumer_object, std);
    uamqp_consumer_object_handlers.free_obj = uamqp_consumer_object_handler_free;

    return SUCCESS;
}

