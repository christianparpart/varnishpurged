#ifndef varnishpurged_h
#define varnishpurged_h (1)

#include <ev++.h>

#define PPURGE_REDIS_SET "varnishpurged:queue"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1

#define POLL_TIMEOUT_INIT 0.10
#define POLL_TIMEOUT_IDLE 0.50
#define POLL_TIMEOUT_BUSY 0.01

#define STR_BUFSIZE 4096

#endif
