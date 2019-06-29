## php-uamqp

```
cd deps/azure-uamqp-c
mkdir cmkae
cd cmake
cmake ..
cmake --build .
cd ../..

phpize
./configure
make
sudo make install
```

To test extension in interactive mode run:

```
php -dextension=uamqp.so -a
```