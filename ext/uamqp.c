/* uamqp extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <openssl/ssl.h>
#include "php.h"
#include "ext/standard/info.h"
#include "php_uamqp.h"
#include "src/php/php_uamqp_connection.h"
#include "src/php/php_uamqp_destination.h"
#include "src/php/php_uamqp_exception.h"
#include "src/php/php_uamqp_message.h"
#include "src/php/php_uamqp_producer.h"
#include "src/php/php_uamqp_consumer.h"

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
    php_info_print_table_header(2, "OpenSSL version", SSLeay_version(SSLEAY_VERSION));
    php_info_print_table_header(2, "OpenSSL Header Version", OPENSSL_VERSION_TEXT);
    php_info_print_table_end();
}

static const zend_function_entry uamqp_functions[] = {
    PHP_FE_END
};

static PHP_MINIT_FUNCTION(uamqp)
{
    PHP_MINIT(uamqp_connection)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(uamqp_destination)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(uamqp_exception)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(uamqp_message)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(uamqp_producer)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(uamqp_consumer)(INIT_FUNC_ARGS_PASSTHRU);

    return SUCCESS;
}

zend_module_entry uamqp_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_UAMQP_EXTNAME,            /* Extension name */
    uamqp_functions,              /* zend_function_entry */
    PHP_MINIT(uamqp),             /* PHP_MINIT - Module initialization */
    NULL,    /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(uamqp),             /* PHP_RINIT - Request initialization */
    NULL,    /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(uamqp),             /* PHP_MINFO - Module info */
    PHP_UAMQP_VERSION,            /* Version */
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_UAMQP
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(uamqp)
#endif
