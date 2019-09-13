--TEST--
General test that is going to send couple of messages into the destination through \UAMQP\Producer and receive them using \UAMQP\Consumer
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

$connection = new \UAMQP\Connection(getenv('PHP_UAMQP_TEST_SB_HOST'), 5671, getenv('PHP_UAMQP_TEST_SB_POLICY_NAME'), getenv('PHP_UAMQP_TEST_SB_POLICY_KEY'));

$producer = new \UAMQP\Producer($connection);
$destination = new \UAMQP\Destination(getenv('PHP_UAMQP_TEST_SB_DESTINATION'));

$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 1 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 2 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 3 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 4 " . time()), $destination);

$consumer = new \UAMQP\Consumer($connection, \UAMQP\Consumer::PEAK_AND_LOCK);

$i = 0;

$consumer->listen(function($message) use (&$i) {
    $i++;

    var_dump($i, $message);

    if ($i >= 4) {
        return \UAMQP\Consumer::STOP;
    }

    return \UAMQP\Consumer::ACCEPT_NEXT;
}, $destination);

$consumer->listen(function($message) {

    var_dump($message);

    return \UAMQP\Consumer::ACCEPT_STOP;
}, $destination);

?>
--EXPECTF--
int(1)
object(UAMQP\Message)#%d (1) {
  ["payload"]=>
  string(%d) "this is some random test message %d %d"
}
int(2)
object(UAMQP\Message)#%d (1) {
  ["payload"]=>
  string(%d) "this is some random test message %d %d"
}
int(3)
object(UAMQP\Message)#%d (1) {
  ["payload"]=>
  string(%d) "this is some random test message %d %d"
}
int(4)
object(UAMQP\Message)#%d (1) {
  ["payload"]=>
  string(%d) "this is some random test message %d %d"
}
object(UAMQP\Message)#%d (1) {
  ["payload"]=>
  string(45) "this is some random test message %d %d"
}