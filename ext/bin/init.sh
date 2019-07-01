#!/usr/bin/env bash

mkdir ./deps/azure-uamqp-c/cmake
cd ./deps/azure-uamqp-c/cmake
cmake ..
cmake --build .
cd ./../../..
phpize
./configure
