#include "varnishpurged.h"
#include "PurgeWorker.h"
#include "ip_addr.h"

PurgeWorker::PurgeWorker(ev::loop_ref& loop_, redis_cfg* redis_config) :
	loop(loop_),
	poll_timer(loop_),
	redis(nullptr)
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


void PurgeWorker::purgeNext(std::string url) {
	// FIXPAUL: re-queue the poll with zero timeout if there was 
	// something to process (work at maximum speed, then sleep)
	// poll_timer.start(POLL_TIMEOUT_BUSY, 0.0);

	poll_timer.start(POLL_TIMEOUT_IDLE, 0.0);
}


void PurgeWorker::onPoll(ev::timer& timer, int revents) {
	printf("polled\n");

	char command[1024] = "SPOP fnord"; // FIXPAUL

	redisAsyncCommand(this->redis, FNORDCAST2 &PurgeWorker::onPollData, this, command);
}


void PurgeWorker::onPollData(redisAsyncContext *redis, void *response, void *privdata) {
	PurgeWorker *self = reinterpret_cast<PurgeWorker *>(privdata);	
	printf("onkeydata\n");

	self->purgeNext("fnord"); // FIXPAUL
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