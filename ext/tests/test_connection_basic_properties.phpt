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
$connection = new \UAMQP\Connection("test.servicebus.windows.net", 5671, true, 'RootManageSharedAccessKey', 'test');
var_dump($connection->host());
?>
--EXPECT--
string(27) "test.servicebus.windows.net"
