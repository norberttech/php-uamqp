##################################################################
# NOTE: This CMake file is just for syntax highlighting in CLion #
##################################################################

cmake_minimum_required(VERSION 3.6)
project(php-uamqp)

message("Begin cmaking of PHP extension ...")

# -std=gnu99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -O0 -Wall -std=gnu99 -fvisibility=hidden")
set(ENV{PROJECT_ROOT} "${CMAKE_HOME_DIRECTORY}")

include_directories(
    ext/php-build/src
    ext/php-build/src/main
    ext/php-build/src/Zend
    ext/php-build/src/TSRM
    ext/php-build/src/ext
    ext/php-build/src/sapi

    ext/deps/azure-uamqp-c/deps/azure-macro-utils-c/inc
    ext/deps/azure-uamqp-c/deps/azure-macro-utils-c/src

    ext/deps/azure-uamqp-c/deps/umock-c/inc
    ext/deps/azure-uamqp-c/deps/umock-c/src
    ext/deps/azure-uamqp-c/deps/azure-c-shared-utility/src
    ext/deps/azure-uamqp-c/deps/azure-c-shared-utility/inc
    ext/deps/azure-uamqp-c/src
    ext/deps/azure-uamqp-c/inc
)

FILE(GLOB KernelHeaders ./kernel/*.h)
FILE(GLOB KernelSources ./kernel/*.c)

set(SOURCE_FILES
    ext/uamqp.c
    ext/php_uamqp.h
    ext/src/php/php_uamqp_connection.c
    ext/src/php/php_uamqp_connection.h
    ext/src/php/php_uamqp_exception.c
    ext/src/php/php_uamqp_exception.h
    ext/src/php/php_uamqp_destination.c
    ext/src/php/php_uamqp_destination.h
    ext/src/php/php_uamqp_message.c
    ext/src/php/php_uamqp_message.h
    ext/src/php/php_uamqp_producer.c
    ext/src/php/php_uamqp_producer.h
    ext/src/php/php_uamqp_consumer.c
    ext/src/php/php_uamqp_consumer.h
    ext/src/uamqp/uamqp.c
    ext/src/uamqp/uamqp.h
    ${KernelHeaders}
    ${KernelSources}
)

if(EXISTS "$ENV{PROJECT_ROOT}/ext/config.h")
    set(SOURCE_FILES "${SOURCE_FILES};ext/config.h")
endif()

add_library(uamqp ${SOURCE_FILES})

message("End cmaking of PHP extension ...")