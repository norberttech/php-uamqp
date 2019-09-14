<?php

$connection = new \UAMQP\Connection(getenv('PHP_UAMQP_TEST_SB_HOST'), 5671, getenv('PHP_UAMQP_TEST_SB_POLICY_NAME'), getenv('PHP_UAMQP_TEST_SB_POLICY_KEY'));

$producer = new \UAMQP\Producer($connection);
$destination = new \UAMQP\Destination(getenv('PHP_UAMQP_TEST_SB_DESTINATION'));

$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 1 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 2 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 3 " . time()), $destination);
$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 4 " . time()), $destination);

$consumer = new \UAMQP\Consumer($connection, \UAMQP\Consumer::RECEIVE_AND_DELETE);

$i = 0;

$consumer->listen(function($message) use (&$i) {
    $i++;

    var_dump($i, $message);

    if ($i >= 4) {
        return false;
    }

    return true;
}, $destination);