#!/usr/bin/env bash

rm -rf ./deps/azure-uamqp-c/cmake
make clean
if [[ -z "$PHP_UAMQP_PHP_EXECUTABLE" ]]; then
    echo "PHP_UAMQP_PHP_EXECUTABLE not found, trying to use php from PATH"

    phpize --clean
else
    echo "Using PHP from PHP_UAMQP_PHP_EXECUTABLE location: ${PHP_UAMQP_PHP_EXECUTABLE}/php"

    ${PHP_UAMQP_PHP_EXECUTABLE}/phpize --clean
fi