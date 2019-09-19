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
var_dump($message);
?>
--EXPECT--
string(30) "This is sample message payload"
object(UAMQP\Message)#1 (1) {
  ["payload"]=>
  string(30) "This is sample message payload"
}