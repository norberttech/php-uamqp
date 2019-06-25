#ifndef PHP_UAMQP_H
#define PHP_UAMQP_H

extern zend_module_entry uamqp_module_entry;
#define phpext_uamqp_ptr &uamqp_module_entry

#define PHP_UAMQP_VERSION "0.0.1"
#define PHP_UAMQP_EXTNAME "uamqp"

#define PHP_UAMQP_NAMESPACE "UAMQP"

#ifdef PHP_WIN32
#	define PHP_UAMQP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_UAMQP_API __attribute__ ((visibility("default")))
#else
#	define PHP_UAMQP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define UAMQP_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(uamqp, v)

#if defined(ZTS) && defined(COMPILE_DL_UAMQP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_UAMQP_H */

