#ifndef varnishpurged_PurgeWorker_h
#define varnishpurged_PurgeWorker_h

#define FNORDCAST1 (void (*)(const redisAsyncContext*, int))
#define FNORDCAST2 (void (*)(redisAsyncContext*, void*, void*))

#define REDIS_DEFAULT_PORT 6379

#include <signal.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libev.h>

#include "redis_cfg.h"

class PurgeWorker {
protected:
	ev::loop_ref& loop;
	ev::timer poll_timer;
	char const *address;
	redisAsyncContext* redis;
	int redisKeyMode;
	char* redisKeyString;

public:
	PurgeWorker(ev::loop_ref& loop_, redis_cfg* redis_config);
	void purgeNext(std::string key);

    static void onPoll(ev_timer* timer, int revents);

	static void onPollData(redisAsyncContext *redis, void *response, void *privdata);
	static void onConnect(const redisAsyncContext* redis, int status);
	static void onDisconnect(const redisAsyncContext* redis, int status);	

private:
	void onPoll(ev::timer& timer, int revents);
};

#endif
