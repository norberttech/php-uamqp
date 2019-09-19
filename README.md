## azure-uaqmp-c php bindings

This extensions brings to php bindings for [Azure uAMQP C Library](https://github.com/Azure/azure-uamqp-c), a general purpose C library for AMQP 1.0.

## PHP supported Versions

| PHP Version   | Supported     |
| ------------- | ------------- |
| 7.2           | ✅            |
| 7.3           | ✅            |
| 7.4           | ✅            |

## Development

Download repository and then initialize submodules (dependencies)

```console
git clone git@github.com:norzechowicz/php-uamqp.git --recursive
cd php-uamqp
git submodule update --init --recursive 
```

[cd ./ext](/ext)

## PHP uAQMP API

Below you can find public API (classes/functions/constants/interfaces) of the extension, since this project is still
under development, those methods might change. Do not use at production, it was never tested anywhere.

### UAMQP\Connection

```php
UAMQP\Connection::__construct(string $host, int $port, string $policy_name, string $policy_key)
UAMQP\Connection::enableDebugMode() : void
UAMQP\Connection::disableDebugMode() : void
UAMQP\Connection::host() : string
UAMQP\Connection::port() : int
UAMQP\Connection::policy_name() : string
UAMQP\Connection::policy_key() : string
```

### UAMQP\Message

```php
UAMQP\Message::__construct(string $payload)
UAMQP\Message::payload() : string
```

### UAMQP\Destination

```php
UAMQP\Destination::__construct(string $value)
UAMQP\Destination::value() : string
```

### UAMQP\Producer

```php
UAMQP\Producer::__construct(UAMQP\Connection $message)
UAMQP\Producer::sendMessage(UAMQP\Message $message, UAMQP\Destination $destination) : void
```

### UAMQP\Consumer

```php
const \UAMQP\Receiver::RECEIVE_AND_DELETE = 0;
const \UAMQP\Receiver::PEAK_AND_LOCK = 1;

\UAMQP\Consumer::__construct(\UAMQP\Connection $message)

\UAMQP\Consumer::open(\UAMQP\Destination $destination, int $settleMode = \UAMQP\Receiver::RECEIVE_AND_DELETE) : void
UAMQP\Consumer::receive() : void
UAMQP\Consumer::accept() : void
UAMQP\Consumer::release() : void
UAMQP\Consumer::reject(string $errorCondition, string $errorDescription) : void
UAMQP\Consumer::close() : void
```

## Examples

Opening connection, creating producer, publishing couple messages and consuming 
them with consumer in `RECEIVE_AND_DELETE` settle mode (messages deleted when received).

```php
<?php

$connection = new \UAMQP\Connection(getenv('PHP_UAMQP_TEST_SB_HOST'), 5671, getenv('PHP_UAMQP_TEST_SB_POLICY_NAME'), getenv('PHP_UAMQP_TEST_SB_POLICY_KEY'));

$producer = new \UAMQP\Producer($connection);
$destination = new \UAMQP\Destination(getenv('PHP_UAMQP_TEST_SB_DESTINATION'));

$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 1 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 2 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 3 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 4 " . time()), $destination);

$consumer = new \UAMQP\Consumer($connection, \UAMQP\Consumer::RECEIVE_AND_DELETE);

$i = 0;

$consumer->open($destination);

for ($t = 0; $t < 10; $t++) {
    $message = $consumer->receive();

    if ($message) {
        $i++;
        var_dump($i, $message);

        continue ;
    }

    if ($i >= 4) {
        break;
    }

    usleep(250000);
}
```