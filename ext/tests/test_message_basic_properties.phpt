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
$message = new \UAMQP\Message("This is sample message payload", "destination");
var_dump($message->payload());
var_dump($message->destination());
?>
--EXPECT--
string(30) "This is sample message payload"
string(11) "destination"
