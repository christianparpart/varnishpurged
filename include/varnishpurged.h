#ifndef varnishpurged_h
#define varnishpurged_h (1)

#include <ev++.h>

#define PPURGE_REDIS_SET "dawanda:product_purger:queue"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1

#define POLL_TIMEOUT_INIT 0.1
#define POLL_TIMEOUT_IDLE 0.2
#define POLL_TIMEOUT_BUSY 0.0

#endif
