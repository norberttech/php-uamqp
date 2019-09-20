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
try {
    $connection = new \UAMQP\Connection("test.servicebus.windows.net", 1234, 'RootManageSharedAccessKey', 'test');
} catch (\UAMQP\Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Invalid port 1234, supported ports [5671]
