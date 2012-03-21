#include "varnishpurged.h"
#include "PurgeWorker.h"

PurgeWorker::PurgeWorker(ev::loop_ref& loop_, redis_cfg* redis_config_, varnish_cfg* varnish_config_) :
	loop(loop_),
	poll_timer(loop_),
	redis(nullptr),
	varnish_config(varnish_config_),
	redis_config(redis_config_)
{
	curl = curl_easy_init();

	if(curl == NULL){
		printf("ERROR: curl init failed\n");
		exit(1);
	}

	poll_timer.set<PurgeWorker, &PurgeWorker::onPoll>(this);
	poll_timer.start(POLL_TIMEOUT_INIT, 0.0);

	printf("connecting to redis on: %s:%i\n", redis_config->host, redis_config->port);

	redis = redisAsyncConnect(redis_config->host, redis_config->port);
	redisLibevAttach(loop, redis);

	redisAsyncSetConnectCallback(redis, FNORDCAST1 &PurgeWorker::onConnect);    
	redisAsyncSetDisconnectCallback(redis, FNORDCAST1 &PurgeWorker::onDisconnect);    

	if (redis->err) {		
		printf("ERROR: %s\n", redis->errstr);	
		exit(1);
	}
}


void PurgeWorker::purgeUrl(char* url){
	char purge_url[STR_BUFSIZE];
	std::string source_url = url;
	int ind = source_url.find("/");

	if(ind == -1)
		return;

	snprintf(
	  purge_url, STR_BUFSIZE, 
	  "http://%s:%i%s", 
	  varnish_config->host,
	  varnish_config->port,
	  (url + ind)
	);

	printf("\npurging: %s -> %s \n", url, purge_url);

	// this is a shortened version of what was previously called from the ruby code:
	// curl --request PURGE --header X-Host:de.dawanda.com product-varnish:8080/product/1234
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	curl_easy_setopt(curl, CURLOPT_URL, purge_url);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 300);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PURGE");

	curl_easy_perform(curl);	
}


void PurgeWorker::purgeNext(char* url_or_null) {
	if(url_or_null == NULL){
		printf(".");
		fflush(stdout);
		poll_timer.start(POLL_TIMEOUT_IDLE, 0.0);
	} else {
		purgeUrl(url_or_null);
		poll_timer.start(POLL_TIMEOUT_BUSY, 0.0);
	}
}


void PurgeWorker::onPoll(ev::timer& timer, int revents) {
	redisAsyncCommand(this->redis, FNORDCAST2 &PurgeWorker::onPollData, this, "SPOP %s", redis_config->skey);
}


void PurgeWorker::onPollData(redisAsyncContext *redis, redisReply *reply, void *privdata) {
	PurgeWorker *self = reinterpret_cast<PurgeWorker *>(privdata);
	
	if (reply == NULL){
		printf("something wen't wrong (empty redis replay), bailing out");
		exit(1);
	}

	self->purgeNext(reply->str);
}


void PurgeWorker::onConnect(const redisAsyncContext* redis, int status) {
	if (status == REDIS_OK) {
		printf("connected, listening for purge urls\n");	
	} else {
		printf("ERROR: %s\n", redis->errstr);	
		exit(1);
	}
}


void PurgeWorker::onDisconnect(const redisAsyncContext* redis, int status) {
	if (status != REDIS_OK) {
		printf("DISCONNECT: %s\n", redis->errstr);
		exit(1);
	}
}