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
$connection = new \UAMQP\Connection("norbert-fulfillment-dev-service-bus.servicebus.windows.net", 5671, true, 'RootManageSharedAccessKey', 'test');

$connection->connect();
?>
--EXPECT--
already connected
