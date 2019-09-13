## azure-uaqmp-c php bindings

This extensions bring to php bindings for [Azure uAMQP C Library](https://github.com/Azure/azure-uamqp-c), a general purpose C library for AMQP 1.0.


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
UAMQP\Connection::__construct(string $host, int $port, string $policyName, string $policyKey)
UAMQP\Connection::enableDebugMode() : void
UAMQP\Connection::disableDebugMode() : void
UAMQP\Connection::host() : string
UAMQP\Connection::port() : int
UAMQP\Connection::policyName() : string
UAMQP\Connection::policyKey() : string
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
const \UAMQP\Receiver::ACCEPT_NEXT = 2;
const \UAMQP\Receiver::STOP = 3;
const \UAMQP\Receiver::ACCEPT_STOP = 4;
const \UAMQP\Receiver::RELEASE = 5;

\UAMQP\Consumer::__construct(\UAMQP\Connection $message)

\UAMQP\Consumer::listen(callable $callback, \UAMQP\Destination $destination, int $settleMode = \UAMQP\Receiver::RECEIVE_AND_DELETE) : void

$callback = function(UAMQP\Message $message) {
    return true; // \UAMQP\Receiver::RECEIVE_AND_DELETE - receive more
}

$callback = function(UAMQP\Message $message) {
    return false; // \UAMQP\Receiver::RECEIVE_AND_DELETE - stop receiving
}

$callback = function(UAMQP\Message $message) {
    return UAMQP\Receiver::ACCEPT_NEXT; // UAMQP\Receiver::RECEIVE_AND_DELETE- receive more
}

$callback = function(UAMQP\Message $message) {
    return UAMQP\Receiver::STOP; // UAMQP\Receiver::RECEIVE_AND_DELETE - stop receiving
}

$callback = function(UAMQP\Message $message) {
    throw new \RuntimeException(); // UAMQP\Receiver::RECEIVE_AND_DELETE - stop receiving
}

$callback = function(UAMQP\Message $message) {
    return true; // UAMQP\Receiver::PEAK_AND_LOCK - accept message and receive more
}

$callback = function(UAMQP\Message $message) {
    return false; // UAMQP\Receiver::PEAK_AND_LOCK - accept message and not receive more
}

$callback = function(UAMQP\Message $message) {
    return UAMQP\Receiver::ACCEPT_NEXT; // UAMQP\Receiver::PEAK_AND_LOCK - accept message and receive more
}

$callback = function(UAMQP\Message $message) {
    return UAMQP\Receiver::ACCEPT_STOP; // UAMQP\Receiver::PEAK_AND_LOCK - accept message and not receive more
}

$callback = function(UAMQP\Message $message) {
    return UAMQP\Receiver::STOP; // UAMQP\Receiver::PEAK_AND_LOCK - rlease message and not receive more
}

$callback = function(UAMQP\Message $message) {
    throw new \RuntimeException(); // UAMQP\Receiver::PEAK_AND_LOCK - reject message and not receive more, exception message will be error description for dead letter
}
```
