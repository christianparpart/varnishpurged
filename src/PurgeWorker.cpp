#include <sstream>
#include "varnishpurged.h"
#include "PurgeWorker.h"
#include "redis_cfg.h"
#include "varnish_cfg.h"
#include <hiredis/adapters/libev.h>

PurgeWorker::PurgeWorker(ev::loop_ref loop_, redis_cfg* redis_config_, varnish_cfg* varnish_config_) :
	loop(loop_),
	poll_timer(loop_),
	redis(nullptr),
	varnish_config(varnish_config_),
	redis_config(redis_config_)
{
	curl = curl_easy_init();

	if(curl == nullptr){
		printf("ERROR: curl init failed\n");
		exit(1);
	}

	poll_timer.set<PurgeWorker, &PurgeWorker::onPoll>(this);
	poll_timer.start(POLL_TIMEOUT_INIT, 0.0);

	printf("connecting to redis on: %s:%i\n", redis_config->host.c_str(), redis_config->port);

	redis = redisAsyncConnect(redis_config->host.c_str(), redis_config->port);
	redisLibevAttach(loop, redis);

	redisAsyncSetConnectCallback(redis, (redisConnectCallback*) &PurgeWorker::onConnect);    
	redisAsyncSetDisconnectCallback(redis, (redisConnectCallback*) &PurgeWorker::onDisconnect);    

	if (redis->err) {
		printf("ERROR: %s\n", redis->errstr);
		exit(1); // TODO FIXPAUL's code. never exit a spaceshuttle just because you feel mad at a callee.
	}
}

PurgeWorker::~PurgeWorker()
{
	// TODO google for RAII and then think about what a destructor might be good for :)
	curl_easy_cleanup(curl);
}

void PurgeWorker::run()
{
	loop.run();
}

void PurgeWorker::purgeUrl(const char* url)
{
	struct curl_slist *headers = nullptr;
	std::string source_url = url;
	size_t ind = source_url.find("/");

	if (ind == std::string::npos)
		return;

	if (ind >= STR_BUFSIZE)
		return;

	std::stringstream  purge_url;
	purge_url << "http://" << varnish_config->host << ":" << varnish_config->port << &url[ind];

	std::string host = source_url.substr(0, ind);
	std::string purge_xhost = "X-Host: " + host;
	headers = curl_slist_append(headers, purge_xhost.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, purge_url.str().c_str());
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PURGE");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, VARNISH_TIMEOUT);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, VARNISH_TIMEOUT);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 10);

	curl_easy_perform(curl);
}

void PurgeWorker::purgeNext(const char* url_or_null)
{
	if (url_or_null == nullptr) {
		printf(".");
		fflush(stdout);
		poll_timer.start(POLL_TIMEOUT_IDLE, 0.0);
	} else {
		purgeUrl(url_or_null);
		poll_timer.start(POLL_TIMEOUT_BUSY, 0.0);
	}
}

void PurgeWorker::onPoll(ev::timer& timer, int revents)
{
	redisAsyncCommand(redis, (redisCallbackFn*)&PurgeWorker::onPollData, this, "SPOP %s", redis_config->skey.c_str());
}

void PurgeWorker::onPollData(redisAsyncContext* redis, redisReply* reply, void* privdata)
{
	PurgeWorker* self = reinterpret_cast<PurgeWorker*>(privdata);

	if (reply == nullptr) {
		printf("something wen't wrong (empty redis replay), bailing out");
		exit(1); // TODO be more fail-safe and let's not crunch our shuttle that soon
	}

	self->purgeNext(reply->str);
}

void PurgeWorker::onConnect(const redisAsyncContext* redis, int status)
{
	if (status == REDIS_OK) {
		printf("connected, listening for purge urls\n");	
	} else {
		printf("ERROR: %s\n", redis->errstr);	
		exit(1); // TODO your spaceshuttle is going to shutdown ungracefully NOW. FIXPAUL's code.
		// FYI if a connect fails, just wait and try later again. queue up pending purges until then.
	}
}

void PurgeWorker::onDisconnect(const redisAsyncContext* redis, int status)
{
	if (status != REDIS_OK) {
		printf("DISCONNECT: %s\n", redis->errstr);
		exit(1);
	}
}
