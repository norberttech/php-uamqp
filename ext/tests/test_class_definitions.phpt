--TEST--
UAMQPConnection() initial state test
--SKIPIF--
<?php
if (!extension_loaded('uamqp')) {
	echo 'skip';
}
?>
--FILE--
<?php

function print_class($className) {
    $reflection = new \ReflectionClass($className);
    echo $reflection->isFinal() ? 'final ' : ''
        . $reflection->isAbstract() ? 'abstract ' : ''
        . $className . "\n";

    echo "------\n";

    foreach ($reflection->getMethods() as $reflectionMethod) {
        $method = $className . "::" . $reflectionMethod->getName() . '(';

        foreach ($reflectionMethod->getParameters() as $index => $reflectionParameter) {
            $method .= ($reflectionParameter->hasType() ? $reflectionParameter->getType()->getName() : '');
            $method .= ' $' . $reflectionParameter->getName();
            if ($reflectionParameter->isDefaultValueAvailable()) {
                $method .= ' = ' . $reflectionParameter->getDefaultValue();
            }

            if ($index < \count($reflectionMethod->getParameters()) - 1) {
                $method .= ', ';
            }
        }

        $method .= ')';

        if (!$reflectionMethod->isConstructor()) {
            $method .= ' :';

            if ($reflectionMethod->hasReturnType()) {
                $method .= $reflectionMethod->getReturnType()->allowsNull() ? ' ?' : ' ';
                $method .= $reflectionMethod->getReturnType()->getName();
            } else {
                $method .= ' void';
            }
        }

        echo $method . "\n";
    }

    echo "------\n";
}

print_class(\UAMQP\Connection::class);
print_class(\UAMQP\Message::class);
print_class(\UAMQP\Producer::class);
?>
--EXPECT--
UAMQP\Connection
------
UAMQP\Connection::__construct(string $host, int $port, bool $useTLS, string $policyName, string $policyKey)
UAMQP\Connection::host() : string
UAMQP\Connection::port() : int
UAMQP\Connection::useTLS() : int
UAMQP\Connection::policyName() : string
UAMQP\Connection::policyKey() : string
------
UAMQP\Message
------
UAMQP\Message::__construct(string $payload, string $destination)
UAMQP\Message::payload() : string
UAMQP\Message::destination() : string
------
UAMQP\Producer
------
UAMQP\Producer::__construct(UAMQP\Connection $message)
UAMQP\Producer::sendMessage(UAMQP\Message $message) : void
------
