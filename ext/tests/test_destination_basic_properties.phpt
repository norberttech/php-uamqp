--TEST--
\UAMQP\Destination class basic properties
--SKIPIF--
<?php
//if (!extension_loaded('uamqp')) {
	echo 'skip';
//}
?>
--FILE--
<?php
$destination = new \UAMQP\Destination("destination_path");
var_dump($destination->value());
var_dump($destination);
?>
--EXPECT--
string(16) "destination_path"
object(UAMQP\Destination)#1 (1) {
  ["destination"]=>
  string(16) "destination_path"
}