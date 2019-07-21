## azure-uaqmp-c php bindings

This extensions bring to php bindings for [Azure uAMQP C Library](https://github.com/Azure/azure-uamqp-c), a general purpose C library for AMQP 1.0.


## Development

Download repository and then initialize submodules (dependencies)

```
git clone git@github.com:norzechowicz/php-uamqp.git
cd php-uamqp
git submodule init && git submodule update --recursive
```

[cd ./ext](/ext)

## PHP uAQMP API

Below you can find public API (classes/functions/constants/interfaces) of the extension, since this project is still
under development, those methods might change. Do not use at production, it was never tested anywhere.

### UAMQP\Connection

```
UAMQP\Connection::__construct(string $host, int $port, string $policyName, string $policyKey)
UAMQP\Connection::enableDebugMode() : void
UAMQP\Connection::disableDebugMode() : void
UAMQP\Connection::host() : string
UAMQP\Connection::port() : int
UAMQP\Connection::policyName() : string
UAMQP\Connection::policyKey() : string
```

### UAMQP\Message

```
UAMQP\Message::__construct(string $payload)
UAMQP\Message::payload() : string
```

### UAMQP\Destination

```
UAMQP\Destination::__construct(string $value)
UAMQP\Destination::value() : string
```

### UAMQP\Producer

```
UAMQP\Producer::__construct(UAMQP\Connection $message)
UAMQP\Producer::sendMessage(UAMQP\Message $message, UAMQP\Destination $destination) : void
```

### UAMQP\Consumer

```
UAMQP\Consumer::__construct(UAMQP\Connection $message)
UAMQP\Consumer::listen(callable $callback, UAMQP\Destination $message) : void

$callback = function(UAMQP\Message $message) {
    return true; // receive more
}

$callback = function(UAMQP\Message $message) {
    return false; // stop receiving
}

$callback = function(UAMQP\Message $message) {
    throw new \RuntimeException(); // stop receiving
}
```
