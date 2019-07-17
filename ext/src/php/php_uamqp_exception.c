#include <php.h>
#include <Zend/zend_exceptions.h>
#include "../../php_uamqp.h"
#include "php_uamqp_exception.h"

zend_class_entry *uamqp_exception_ce;

zend_class_entry *php_uamqp_exception_ce(void)
{
    return uamqp_exception_ce;
}

PHP_MINIT_FUNCTION(uamqp_exception)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_UAMQP_NAMESPACE, PHP_UAMQP_EXCEPTION_CLASS, NULL);
    uamqp_exception_ce = zend_register_internal_class_ex(&ce, zend_ce_exception);
    uamqp_exception_ce->create_object = zend_ce_exception->create_object;

    return SUCCESS;
}

