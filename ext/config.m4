PHP_ARG_ENABLE([uamqp],
  [whether to enable uamqp support],
  [AS_HELP_STRING([--enable-uamqp],
    [Enable uamqp support])],
  [no])

if test "$PHP_UAMQP" != "no"; then
	AC_DEFINE(HAVE_UAMQP, 1, [ uamqp support enable ])

	PHP_NEW_EXTENSION(uamqp, uamqp.c src/uamqp_connection.c, $ext_shared)
    echo $ext_srcdir

    PHP_ADD_INCLUDE([$ext_srcdir/deps/umock-c/inc])
    PHP_ADD_INCLUDE([$ext_srcdir/deps/umock-c/deps/azure-macro-utils-c/inc])
    PHP_ADD_INCLUDE([$ext_srcdir/deps/azure-uamqp-c/inc])
    PHP_ADD_INCLUDE([$ext_srcdir/deps/azure-c-shared-utility/inc])


    CFLAGS=" $CFLAGS -Wl,deps/umock-c/cmake/libumock_c.a"
    CFLAGS=" $CFLAGS -Wl,deps/azure-c-shared-utility/cmake/libaziotsharedutil.a"
    CFLAGS=" $CFLAGS -Wl,deps/azure-uamqp-c/cmake/libuamqp.a"

    PHP_ADD_LIBPATH($ext_srcdir/deps/azure-uamqp-c/cmake, UAMQP_SHARED_LIBADD)
    PHP_ADD_LIBPATH($ext_srcdir/deps/azure-c-shared-utility/cmake, ACSHAREDUTIL_SHARED_LIBADD)
    PHP_ADD_LIBPATH($ext_srcdir/deps/umock-c/cmake, UMOCKC_SHARED_LIBADD)

    PHP_SUBST([CFLAGS])

fi
