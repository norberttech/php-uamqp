#include <php.h>
#include <Zend/zend_exceptions.h>
#include "php_uamqp_exception.h"

void php_uamqp_throw_exception(char *message, int code)
{
    zend_throw_exception(php_uamqp_exception_ce, message, code);
}

PHP_MINIT_FUNCTION(uamqp_exception)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_EXCEPTION_CLASS, NULL);
    php_uamqp_exception_ce = zend_register_internal_class_ex(&ce, zend_ce_exception);
    php_uamqp_exception_ce->create_object = zend_ce_exception->create_object;

    return SUCCESS;
}

