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
try {
    $message = new \UAMQP\Message(str_repeat("a", 1048576 + 1));
} catch (\UAMQP\Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Message payload can't be longer than 1048576 characters, got 1048577.