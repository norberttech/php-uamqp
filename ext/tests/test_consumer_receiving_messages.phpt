--TEST--
\UAQMP\Producer() sending message to destination
--SKIPIF--
<?php
if (!extension_loaded('uamqp')) {
	echo 'skip';
}

if (!getenv('PHP_UAMQP_TEST_SB_POLICY_NAME')) {
    echo 'skip';
}

?>
--FILE--
<?php

$connection = new \UAMQP\Connection(getenv('PHP_UAMQP_TEST_SB_HOST'), 5671, true, getenv('PHP_UAMQP_TEST_SB_POLICY_NAME'), getenv('PHP_UAMQP_TEST_SB_POLICY_KEY'));

$producer = new \UAMQP\Producer($connection);
$destination = new \UAMQP\Destination("testname");

$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 1 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 2 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 3 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 4 " . time()), $destination);

$consumer = new \UAMQP\Consumer($connection);

$i = 0;

$consumer->listen(function($message) use (&$i) {
    $i++;

    var_dump($i, $message);

    if ($i >= 4) {
        return false;
    }

    return true;
}, $destination);

?>
--EXPECTF--
int(1)
string(44) "this is some random test message %d %d"
int(2)
string(44) "this is some random test message %d %d"
int(3)
string(44) "this is some random test message %d %d"
int(4)
string(44) "this is some random test message %d %d"
