#!/usr/bin/env bash

mkdir ./deps/azure-uamqp-c/cmake
cd ./deps/azure-uamqp-c/cmake
cmake ..
cmake --build .
cd ./../../..
if [[ -z "$PHP_UAMQP_PHP_EXECUTABLE" ]]; then
    echo "PHP_UAMQP_PHP_EXECUTABLE not found, trying to use php from PATH"

    phpize
    ./configure
else
    echo "Using PHP from PHP_UAMQP_PHP_EXECUTABLE location: ${PHP_UAMQP_PHP_EXECUTABLE}/php"

    ${PHP_UAMQP_PHP_EXECUTABLE}/phpize
    ./configure --with-php-config=${PHP_UAMQP_PHP_EXECUTABLE}/php-config
fi
