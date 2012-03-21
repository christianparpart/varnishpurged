#ifndef varnishpurged_PurgeWorker_h
#define varnishpurged_PurgeWorker_h

#define FNORDCAST1 (void (*)(const redisAsyncContext*, int))
#define FNORDCAST2 (void (*)(redisAsyncContext*, void*, void*))

#include <signal.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libev.h>
#include <curl/curl.h>

#include "redis_cfg.h"
#include "varnish_cfg.h"

class PurgeWorker {
protected:
	ev::loop_ref& loop;
	ev::timer poll_timer;
	redisAsyncContext* redis;
	varnish_cfg* varnish_config;
	redis_cfg* redis_config;
	CURL *curl;

public:
	PurgeWorker(ev::loop_ref& loop_, redis_cfg* redis_config, varnish_cfg* varnish_config);
	void purgeUrl(char* url);
	void purgeNext(char* url_or_null);

    static void onPoll(ev_timer* timer, int revents);

	static void onPollData(redisAsyncContext *redis, redisReply *response, void *privdata);
	static void onConnect(const redisAsyncContext* redis, int status);
	static void onDisconnect(const redisAsyncContext* redis, int status);	

private:
	void onPoll(ev::timer& timer, int revents);
};

#endif
