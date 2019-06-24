#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/php_var.h>
#include "uamqp_connection.h"
#include "azure_c_shared_utility/platform.h"

zend_class_entry *uamqp_connection_class_entry;
#define this_ce uamqp_connection_class_entry

PHP_METHOD(UAMQPConnection, isConnected)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    RETURN_BOOL(zval_get_long(zend_read_property(this_ce, getThis(), "connected", sizeof("connected") -1, 1, NULL)));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry uamqp_connection_class_functions[] = {
    PHP_ME(UAMQPConnection, isConnected, arginfo_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(uamqp_connection) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "UAMQPConnection", uamqp_connection_class_functions);

    this_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_bool(this_ce, "connected", sizeof("connected") - 1, 0, ZEND_ACC_PRIVATE);

    return SUCCESS;
}
