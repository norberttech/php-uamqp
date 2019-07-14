--TEST--
\UAQMP\Producer sending message to destination in debug mode
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
$connection->enableDebugMode();

$producer = new \UAMQP\Producer($connection);
$destination = new \UAMQP\Destination("testname");

$producer->sendMessage(new \UAMQP\Message($payload = "this is some random test message 1 " . time()), $destination);

?>
--EXPECTF--
-> Header (AMQP 0.1.0.0)
<- Header (AMQP 0.1.0.0)
-> [OPEN]* {php-uaqmp-binding,%s,4294967295,65535}
<- [OPEN]* {%s,NULL,65536,4999,240000,NULL,NULL,NULL,NULL,NULL}
-> [BEGIN]* {NULL,0,2147483647,65536,4294967295}
<- [BEGIN]* {0,1,5000,2147483647,255,NULL,NULL,NULL}
-> [ATTACH]* {sender-link,0,false,1,0,* {ingress},* {amqps://%s},NULL,NULL,0,65536}
<- [ATTACH]* {sender-link,0,true,1,0,* {ingress,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},* {amqps://%s,NULL,NULL,NULL,NULL,NULL,NULL},NULL,NULL,NULL,65536,NULL,NULL,NULL}
<- [FLOW]* {0,5000,1,2147483647,0,0,1000,0,NULL,false,NULL}
-> [TRANSFER]* {0,0,<01 00 00 00>,0,true,false}
-> [DETACH]* {0,true}
-> [END]* {}
-> [CLOSE]* {}
