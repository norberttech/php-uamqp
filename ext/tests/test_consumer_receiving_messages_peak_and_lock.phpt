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

if ((int) getenv('PHP_UAMQP_TEST_FUNCTIONAL_SKIP') === 1) {
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

$consumer->open($destination);

for ($t = 0; $t < 30; $t++) {
    $message = $consumer->receive();

    if ($message) {
        $i++;
        var_dump($i, $message);
        $consumer->accept();

        continue ;
    }

    if ($i >= 4) {
        break;
    }

    usleep(250000);
}

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