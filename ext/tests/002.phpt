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
$connection = new UAMQPConnection();

var_dump($connection->isConnected());
?>
--EXPECT--
bool(false)
