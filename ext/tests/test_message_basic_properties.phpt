--TEST--
\UAMQP\Message class basic properties
--SKIPIF--
<?php
if (!extension_loaded('uamqp')) {
	echo 'skip';
}
?>
--FILE--
<?php
$message = new \UAMQP\Message("This is sample message payload");
var_dump($message->payload());
?>
--EXPECT--
string(30) "This is sample message payload"
