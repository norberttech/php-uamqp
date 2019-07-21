#include <php.h>
#include <string.h>
#include <ext/standard/php_var.h>
#include <Zend/zend_interfaces.h>
#include "php_uamqp_producer.h"
#include "php_uamqp_connection.h"
#include "php_uamqp_message.h"
#include "php_uamqp_destination.h"

#define PHP_UAMQP_CONSUMER_CLASS "Consumer"

#define METHOD(name) PHP_METHOD(UAMQPConsumer, name)
#define ME(name, arginfo, visibility) PHP_ME(UAMQPConsumer, name, arginfo, visibility)

#define UAMQP_CONSUMER_OBJECT(obj) \
    (uamqp_consumer_object *)((char *) Z_OBJ_P(obj) - Z_OBJ_P(obj)->handlers->offset)

zend_class_entry *uamqp_consumer_ce;
zend_object_handlers uamqp_consumer_object_handlers;

zend_fcall_info listen_method_callback;
zend_fcall_info_cache listen_method_callback_cache;

typedef struct _uamqp_consumer_object {
    uamqp_connection_object *uamqp_connection;
    zend_object zendObject;
} uamqp_consumer_object;

static inline uamqp_consumer_object *php_uamqp_consumer_fetch_object(zend_object *obj) {
    return (uamqp_consumer_object *)((char*)(obj) - XtOffsetOf(uamqp_consumer_object, zendObject));
}

METHOD(__construct)
{
    zval *connection_argument;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1);
        Z_PARAM_OBJECT_OF_CLASS_EX(connection_argument, php_uamqp_connection_ce(), 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    uamqp_consumer_object *object = UAMQP_CONSUMER_OBJECT(getThis());

    object->uamqp_connection = UAMQP_CONNECTION_OBJECT(connection_argument);
}

bool callback(char *msg)
{
    zval callback_result, callback_argument;
    int callback_return;
    zval message_object, constructor_name, constructor_args[1], constructor_result;

    // create function name for call_user_function
    ZVAL_STRING(&constructor_name, ZEND_CONSTRUCTOR_FUNC_NAME);

    // initialize value (zval) of first constructor argument from string
    ZVAL_STRING(&constructor_args[0], msg);

    //Initialize UAMQP\Message object
    object_init_ex(&message_object, php_uamqp_message_ce());

    // execute UAMQP\Message object constructor
    call_user_function(NULL, &message_object, &constructor_name, &constructor_result, 1, constructor_args);

    // Prepare callback
    listen_method_callback.param_count = 1;
    listen_method_callback.params = &message_object;
    listen_method_callback.no_separation = 0;
    listen_method_callback.retval = &callback_result;

    // call listener callback with message object passed as first argument
    callback_return = zend_call_function(&listen_method_callback, &listen_method_callback_cache);

    // cleanup
    i_zval_ptr_dtor(&callback_argument ZEND_FILE_LINE_CC);
    zval_ptr_dtor(&message_object);
    zval_ptr_dtor(&constructor_name);
    zval_ptr_dtor(&constructor_args[0]);
    zval_ptr_dtor(&constructor_result);

    // stop next message consumption if exception
    if (EG(exception)) {
        return true;
    }

    // stop next message consumption if callback did not returned SUCCESS
    if (callback_return != SUCCESS || Z_TYPE(callback_result) == IS_UNDEF) {
        return false;
    }

    // do not stop message consumption if callback returned true
    if (Z_TYPE(callback_result) == IS_TRUE) {
        return false;
    }

    // stop message consumption, callback did not return anything or returned false
    return true;
}

METHOD(listen)
{
    uamqp_consumer_object *object;
    uamqp_destination_object *destination;
    zval *destination_argument;

    listen_method_callback = empty_fcall_info;
    listen_method_callback_cache = empty_fcall_info_cache;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2);
        Z_PARAM_FUNC_EX(listen_method_callback, listen_method_callback_cache, 1, 0)
        Z_PARAM_OBJECT_OF_CLASS_EX(destination_argument, php_uamqp_destination_ce(), 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    object = UAMQP_CONSUMER_OBJECT(getThis());
    destination = UAMQP_DESTINATION_OBJECT(destination_argument);

    uamqp_open_receiver(
        object->uamqp_connection->uamqp_connection,
        create_message_receiver(object->uamqp_connection->uamqp_session, ZSTR_VAL(object->uamqp_connection->properties.host), ZSTR_VAL(destination->value)),
        callback
    );
}

ZEND_BEGIN_ARG_INFO_EX(consumer_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, message, UAMQP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(consumer_listen_arginfo, 0, 0, 2)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
    ZEND_ARG_OBJ_INFO(0, message, UAMQP\\Destination, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_consumer_class_functions[] = {
    ME(__construct, consumer_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ME(listen, consumer_listen_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_object *uamqp_consumer_handler_create(zend_class_entry *ce)
{
    uamqp_consumer_object *consumer = ecalloc(1, sizeof(uamqp_consumer_object) + zend_object_properties_size(ce));

    zend_object_std_init(&consumer->zendObject, ce);
    object_properties_init(&consumer->zendObject, ce);
    consumer->zendObject.handlers = &uamqp_consumer_object_handlers;

    return &consumer->zendObject;
}

void uamqp_consumer_object_handler_free(zend_object *object)
{
    uamqp_consumer_object *consumer = php_uamqp_consumer_fetch_object(object);

    consumer->uamqp_connection = NULL;
    zend_object_std_dtor(&consumer->zendObject);
}

PHP_MINIT_FUNCTION(uamqp_consumer) {
    zend_class_entry ce_consumer;

    INIT_NS_CLASS_ENTRY(ce_consumer, PHP_UAMQP_NAMESPACE, PHP_UAMQP_CONSUMER_CLASS, uamqp_consumer_class_functions);

    ce_consumer.create_object = uamqp_consumer_handler_create;

    uamqp_consumer_ce = zend_register_internal_class(&ce_consumer);

    memcpy(&uamqp_consumer_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    uamqp_consumer_object_handlers.offset = XtOffsetOf(uamqp_consumer_object, zendObject);
    uamqp_consumer_object_handlers.free_obj = uamqp_consumer_object_handler_free;

    return SUCCESS;
}

