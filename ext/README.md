## php-uamqp

```
./init.sh
phpize
./configure
make
make test
```

To install extension after all tests are passing execute:

```
sudo make intall
```

To test extension in interactive mode run:

```
php -dextension=uamqp.so -a
```