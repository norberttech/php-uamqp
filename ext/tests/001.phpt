--TEST--
Check if uamqp is loaded
--SKIPIF--
<?php
if (!extension_loaded('uamqp')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "uamqp" is available';
?>
--EXPECT--
The extension "uamqp" is available
