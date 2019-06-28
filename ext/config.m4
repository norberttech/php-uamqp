PHP_ARG_ENABLE([uamqp],
  [whether to enable uamqp support],
  [AS_HELP_STRING([--enable-uamqp],
    [Enable uamqp support])],
  [no])

if test "$PHP_UAMQP" != "no"; then
	AC_DEFINE(HAVE_UAMQP, 1, [ uamqp support enable ])

    PHP_ADD_INCLUDE([deps/azure-uamqp-c/deps/azure-macro-utils-c/inc])
    PHP_ADD_INCLUDE([deps/azure-uamqp-c/deps/umock-c/inc])
    PHP_ADD_INCLUDE([deps/azure-uamqp-c/deps/azure-c-shared-utility/inc])
    PHP_ADD_INCLUDE([deps/azure-uamqp-c/inc])

	PHP_NEW_EXTENSION(uamqp, uamqp.c src/php/php_uamqp_connection.c src/uamqp/uamqp_connection.c, $ext_shared)

    CFLAGS=" $CFLAGS -Wl,deps/azure-uamqp-c/cmake/deps/umock-c/libumock_c.a"
    CFLAGS=" $CFLAGS -Wl,deps/azure-uamqp-c/cmake/deps/azure-c-shared-utility/libaziotsharedutil.a"
    CFLAGS=" $CFLAGS -Wl,deps/azure-uamqp-c/cmake/libuamqp.a"

    PHP_SUBST([CFLAGS])
fi
