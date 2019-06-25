--TEST--
UAMQPConnection() connection test
--SKIPIF--
<?php
if (!extension_loaded('uamqp')) {
	echo 'skip';
}
?>
--FILE--
<?php
$connection = new \UAMQP\Connection();

$connection->connect();
$connection->connect();
?>
--EXPECT--
already connected
