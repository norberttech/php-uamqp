#include "php_uamqp.h"

PHP_MINIT_FUNCTION(uamqp_consumer);

enum RECEIVER_SETTLE_MODE {
    RECEIVE_AND_DELETE = 0,
    PEAK_AND_LOCK = 1
};