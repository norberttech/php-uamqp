--TEST--
UAMQP\Connection basic properties
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
var_dump($connection->port());
var_dump($connection->useTLS());
var_dump($connection->policyName());
var_dump($connection->policyKey());
?>
--EXPECT--
string(27) "test.servicebus.windows.net"
int(5671)
bool(true)
string(25) "RootManageSharedAccessKey"
string(4) "test"
