#ifndef varnishpurged_h
#define varnishpurged_h (1)

#define REDIS_QUEUE_KEY "varnishpurged:queue"

#define POLL_TIMEOUT_INIT 0.10
#define POLL_TIMEOUT_IDLE 0.50
#define POLL_TIMEOUT_BUSY 0.01

#define STR_BUFSIZE 4096

#define VARNISH_TIMEOUT 300

#endif
