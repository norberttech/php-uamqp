#include "../uamqp/uamqp.h"

#define PHP_UAMQP_NAMESPACE "UAMQP"
#define PHP_UAMQP_CONNECTION_CLASS "Connection"
#define PHP_UAMQP_CONSUMER_CLASS "Consumer"
#define PHP_UAMQP_DESTINATION_CLASS "Destination"
#define PHP_UAMQP_EXCEPTION_CLASS "Exception"
#define PHP_UAMQP_MESSAGE_CLASS "Message"
#define PHP_UAMQP_PRODUCER_CLASS "Producer"

zend_class_entry *php_uamqp_connection_ce;
zend_class_entry *php_uamqp_consumer_ce;
zend_class_entry *php_uamqp_destination_ce;
zend_class_entry *php_uamqp_exception_ce;
zend_class_entry *php_uamqp_message_ce;
zend_class_entry *php_uamqp_producer_ce;

typedef struct _uaqmp_connection_object_properties {
    char *host;
    long port;
    char *policy_name;
    char *policy_key;
} php_uaqmp_connection_object_properties;

typedef struct _uamqp_connection_object {
    php_uaqmp_connection_object_properties properties;
    struct uamqp uamqp_connection;
    struct uamqp_session uamqp_session;
    zend_object std;
} php_uamqp_connection_object;

typedef struct _uamqp_producer_object {
    php_uamqp_connection_object *uamqp_connection;
    zend_object std;
} uamqp_producer_object;

typedef struct _uamqp_consumer_object {
    php_uamqp_connection_object *uamqp_connection;
    long settle_mode;
    zend_object std;
} uamqp_consumer_object;

typedef struct _uamqp_destination_object {
    char *value;
    zend_object std;
} php_uamqp_destination_object;

typedef struct _php_uamqp_message_object {
    char *payload;
    zend_object std;
} php_uamqp_message_object;

static inline php_uamqp_connection_object *php_uamqp_connection_fetch_object(zend_object *obj) {
    return (php_uamqp_connection_object *)((char*)(obj) - XtOffsetOf(php_uamqp_connection_object, std));
}

static inline uamqp_producer_object *php_uamqp_producer_fetch_object(zend_object *obj) {
    return (uamqp_producer_object *)((char*)(obj) - XtOffsetOf(uamqp_producer_object, std));
}

static inline uamqp_consumer_object *php_uamqp_consumer_fetch_object(zend_object *obj) {
    return (uamqp_consumer_object *)((char*)(obj) - XtOffsetOf(uamqp_consumer_object, std));
}

static inline php_uamqp_destination_object *php_uamqp_destination_fetch_object(zend_object *obj) {
    return (php_uamqp_destination_object *)((char*)(obj) - XtOffsetOf(php_uamqp_destination_object, std));
}

static inline php_uamqp_message_object *php_uamqp_message_fetch_object(zend_object *obj) {
    return (php_uamqp_message_object *)((char*)(obj) - XtOffsetOf(php_uamqp_message_object, std));
}