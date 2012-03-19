#ifndef dwn_ppurge_PurgeWorker_h
#define dwn_ppurge_PurgeWorker_h

#define FNORDCAST1 (void (*)(const redisAsyncContext*, int))
#define FNORDCAST2 (void (*)(redisAsyncContext*, void*, void*))

#define REDIS_DEFAULT_PORT 6379

#include <signal.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libev.h>

#include <ip_addr.h>

class PurgeWorker {

protected:
	ev::loop_ref& loop;
	char const *address;
	redisAsyncContext* redis;
	int redisKeyMode;
	char* redisKeyString;

public:
	PurgeWorker(ev::loop_ref& loop_, const char* address_);
    void purgeNext();	
	void purgeURL(std::string key);
	ip_addr* parseAddress(const char* address);

    static void onPoll(ev_timer* timer, int revents);

	static void onKeydata(redisAsyncContext *redis, void *response, void *privdata);
	static void onConnect(const redisAsyncContext* redis, int status);
	static void onDisconnect(const redisAsyncContext* redis, int status);	
};

#endif
