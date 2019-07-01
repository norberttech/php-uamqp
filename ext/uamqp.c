/* uamqp extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_uamqp.h"
#include "src/php/php_uamqp_connection.h"
#include "src/php/php_uamqp_message.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

PHP_RINIT_FUNCTION(uamqp)
{
#if defined(ZTS) && defined(COMPILE_DL_UAMQP)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}

PHP_MINFO_FUNCTION(uamqp)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "uamqp support", "enabled");
    php_info_print_table_end();
}

static const zend_function_entry uamqp_functions[] = {
    PHP_FE_END
};


static PHP_MINIT_FUNCTION(uamqp)
{
    PHP_MINIT(uamqp_connection)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(uamqp_message)(INIT_FUNC_ARGS_PASSTHRU);

    return SUCCESS;
}

zend_module_entry uamqp_module_entry = {
    STANDARD_MODULE_HEADER,
    "uamqp",                      /* Extension name */
    uamqp_functions,              /* zend_function_entry */
    PHP_MINIT(uamqp),             /* PHP_MINIT - Module initialization */
    NULL,                         /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(uamqp),             /* PHP_RINIT - Request initialization */
    NULL,                         /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(uamqp),             /* PHP_MINFO - Module info */
    PHP_UAMQP_VERSION,             /* Version */
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_UAMQP
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(uamqp)
#endif
