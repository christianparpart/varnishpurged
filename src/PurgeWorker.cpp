#include "varnishpurged.h"
#include "PurgeWorker.h"

PurgeWorker::PurgeWorker(ev::loop_ref& loop_, redis_cfg* redis_config_) :
	loop(loop_),
	poll_timer(loop_),
	redis(nullptr),
	redis_config(redis_config_)
{
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
    // this is a shortened version of what was previously called from the ruby code:
    // curl --request PURGE --header X-Host:de.dawanda.com product-varnish:8080/product/1234
    
	printf("\npurging: %s \n", url);
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