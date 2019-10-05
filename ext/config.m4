PHP_ARG_ENABLE([uamqp],
  [whether to enable uamqp support],
  [AS_HELP_STRING([--enable-uamqp],
    [Enable uamqp support])],
  [no])

PHP_ARG_ENABLE(openssl, enable openssl support,
[  --enable-openssl          Use openssl?], no, no)

if test "$PHP_UAMQP" != "no"; then
	AC_DEFINE(HAVE_UAMQP, 1, [ uamqp support enable ])

    if test "$PHP_OPENSSL" != "no" || test "$PHP_OPENSSL_DIR" != "no"; then
        if test "$PHP_OPENSSL_DIR" != "no"; then
            AC_DEFINE(HAVE_OPENSSL, 1, [have openssl])
            PHP_ADD_INCLUDE("${PHP_OPENSSL_DIR}/include")
            PHP_ADD_LIBRARY_WITH_PATH(ssl, "${PHP_OPENSSL_DIR}/${PHP_LIBDIR}")
        else
            AC_CHECK_LIB(ssl, SSL_connect, AC_DEFINE(HAVE_OPENSSL, 1, [have openssl]))
        fi

        AC_DEFINE(UAMQP_USE_OPENSSL, 1, [enable openssl support])
        PHP_ADD_LIBRARY(ssl, 1, UAMQP_SHARED_LIBADD)
        PHP_ADD_LIBRARY(crypto, 1, UAMQP_SHARED_LIBADD)
    fi

    PHP_ADD_INCLUDE([deps/azure-uamqp-c/deps/azure-macro-utils-c/inc])
    PHP_ADD_INCLUDE([deps/azure-uamqp-c/deps/umock-c/inc])
    PHP_ADD_INCLUDE([deps/azure-uamqp-c/deps/azure-c-shared-utility/inc])
    PHP_ADD_INCLUDE([deps/azure-uamqp-c/inc])

	PHP_NEW_EXTENSION(uamqp, uamqp.c src/php/php_uamqp_connection.c src/php/php_uamqp_exception.c src/php/php_uamqp_producer.c src/php/php_uamqp_consumer.c src/php/php_uamqp_message.c src/php/php_uamqp_destination.c src/uamqp/uamqp.c, $ext_shared)

    CFLAGS=" $CFLAGS -Wl,deps/azure-uamqp-c/cmake/deps/umock-c/libumock_c.a"
    CFLAGS=" $CFLAGS -Wl,deps/azure-uamqp-c/cmake/deps/azure-c-shared-utility/libaziotsharedutil.a"
    CFLAGS=" $CFLAGS -Wl,deps/azure-uamqp-c/cmake/libuamqp.a"

    PHP_SUBST([CFLAGS])
fi
