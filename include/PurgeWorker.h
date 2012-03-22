#ifndef varnishpurged_PurgeWorker_h
#define varnishpurged_PurgeWorker_h

#include <signal.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libev.h>
#include <curl/curl.h>
#include <ev++.h>

struct varnish_cfg;
struct redis_cfg;

class PurgeWorker {
protected:
	ev::loop_ref loop;
	ev::timer poll_timer;
	redisAsyncContext* redis;
	varnish_cfg* varnish_config;
	redis_cfg* redis_config;
	CURL* curl;

public:
	PurgeWorker(ev::loop_ref loop_, redis_cfg* redis_config, varnish_cfg* varnish_config);
	~PurgeWorker();

	void run();

	void purgeUrl(const char* url);
	void purgeNext(const char* url_or_null);

private:
	static void onPollData(redisAsyncContext *redis, redisReply *response, void *privdata);
	static void onConnect(const redisAsyncContext* redis, int status);
	static void onDisconnect(const redisAsyncContext* redis, int status);	

	void onPoll(ev::timer& timer, int revents);
};

#endif
