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
$connection = new \UAMQP\Connection("norbert-fulfillment-dev-service-bus.servicebus.windows.net", 5671, true, 'RootManageSharedAccessKey', 'test');
var_dump($connection->isConnected());
var_dump($connection->host());
?>
--EXPECT--
bool(false)
string(58) "norbert-fulfillment-dev-service-bus.servicebus.windows.net"
