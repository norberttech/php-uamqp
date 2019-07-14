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
$message = new \UAMQP\Destination("destination");
var_dump($message->value());
?>
--EXPECT--
string(11) "destination"
