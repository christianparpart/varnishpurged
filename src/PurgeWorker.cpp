#include "varnishpurged.h"
#include "PurgeWorker.h"
#include "ip_addr.h"

PurgeWorker::PurgeWorker(ev::loop_ref& loop_, char const *address_) :
	loop(loop_),
	poll_timer(loop_),
	address(address_),
	redis(nullptr),
	redisKeyMode(),
	redisKeyString()
{
	ip_addr* addr = parseAddress(address_);

	poll_timer.set<PurgeWorker, &PurgeWorker::onPoll>(this);
	poll_timer.start(POLL_TIMEOUT_INIT, 0.0);

	printf("connecting to redis on: %s:%i\n", addr->host, addr->port);

	redis = redisAsyncConnect(addr->host, addr->port);
	redisLibevAttach(loop, redis);

	redisAsyncSetConnectCallback(redis, FNORDCAST1 &PurgeWorker::onConnect);    
	redisAsyncSetDisconnectCallback(redis, FNORDCAST1 &PurgeWorker::onDisconnect);    

	if (redis->err) {		
		printf("ERROR: %s\n", redis->errstr);	
		exit(1);
	}
}

void PurgeWorker::onPoll(ev::timer& timer, int revents) {
	printf("polled\n");

	// FIXPAUL: re-queue the poll with zero timeout if there was 
	// something to process (work at maximum speed, then sleep)
	// poll_timer.start(POLL_TIMEOUT_BUSY, 0.0);

	poll_timer.start(POLL_TIMEOUT_IDLE, 0.0);
}


void PurgeWorker::onKeydata(redisAsyncContext *redis, void* response, void* privdata) {
	PurgeWorker *self = reinterpret_cast<PurgeWorker *>(privdata);	
	printf("onkeydata\n");
	//self->purgeMatchingKeys(response);  
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

ip_addr* PurgeWorker::parseAddress(const char* address_) {
	std::string address = address_;
	ip_addr* addr = (ip_addr *)malloc(sizeof(ip_addr));
	int ind = address.find(":");

	if(ind == -1){
		addr->port = 0; 
		strncpy(addr->host, address.c_str(), sizeof(addr->host));   
	} else {
		addr->port = atoi(address.substr(ind+1).c_str());   
		strncpy(addr->host, address.substr(0, ind).c_str(), sizeof(addr->host));    
	}

	if (addr->port == 0){
		addr->port = REDIS_DEFAULT_PORT;
	}

	return addr;
}
