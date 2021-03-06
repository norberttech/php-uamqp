## php-uamqp

```console
./init.sh
make test
```

To install extension after all tests are passing execute:

```console
sudo make intall
```

To test extension in interactive mode run:

```console
php -dextension=module/uamqp.so -a
```

## Development

Below you can find couple useful tips about php extensions development. 


### Run one of the examples

```console
$PHP_UAMQP_PHP_EXECUTABLE/php -dextension=modules/uamqp.so examples/publish_and_receive_to_service_bus.php
```

### Run examples in gdb 

```console
gdb $PHP_UAMQP_PHP_EXECUTABLE/php
```

Then in gdb 

```console
run -dextension=modules/uamqp.so examples/publish_and_receive_to_service_bus.php
```

$PHP_UAMQP_PHP_EXECUTABLE/php --args -dextension=modules/uamqp.so examples/publish_and_receive_to_service_bus.php

### Browsing core dumps (Vagrant)

By default in vagrant core dumps are saved in `/tmp/cores/` directory. In case you 
run into "Core Dumped" error list the `/tmp/cores` location, get latest dump and browse it
using 

```
gdb $PHP_UAMQP_PHP_EXECUTABLE/php /tmp/cores/core.php.8460.vagrant.1568481785
```

(replace `core.php.8460.vagrant.1568481785` with core dump file path)

and then in gdb console type `bt`.

### Running tests without interaction 

```console
make test NO_INTERACTION=1
```

### Running single test
 
```console
make test TESTS=tests/test_sending_message_in_debug_mode.phpt
```

### Changing test timeout

This will change single test timeout from 60 to 10 seconds. 

```console
make test TEST_TIMEOUT=10
```