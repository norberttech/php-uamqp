#include "php_uamqp.h"

#define PHP_UAMQP_EXCEPTION_CLASS "Exception"

PHP_MINIT_FUNCTION(uamqp_exception);

zend_class_entry *php_uamqp_exception_ce;

void php_uamqp_throw_exception(char *message, int code);