#include "dwn_ppurge.h"
#include "PurgeWorker.h"
#include "ip_addr.h"

PurgeWorker::PurgeWorker(ev::loop_ref& loop_, char const *address_) : loop(loop_) {
	ip_addr* addr = parseAddress(address_);

	ev::timer poll_timer(loop_);
	poll_timer.set<PurgeWorker, &PurgeWorker::onPoll>(this);
	poll_timer.start(5.5, 0.0);

	printf("connecting to redis on: %s:%i\n", addr->host, addr->port);

	this->redis = redisAsyncConnect(addr->host, addr->port);
	redisLibevAttach(this->loop, this->redis);

	redisAsyncSetConnectCallback(this->redis, FNORDCAST1 &PurgeWorker::onConnect);    
	redisAsyncSetDisconnectCallback(this->redis, FNORDCAST1 &PurgeWorker::onDisconnect);    

	if (redis->err) {		
		printf("ERROR: %s\n", this->redis->errstr);	
		exit(1);
	}
}

void PurgeWorker::onPoll(ev::timer& timer, int revents) {
	printf("polled\n");
}


void PurgeWorker::onKeydata(redisAsyncContext *redis, void* response, void* privdata) {
	PurgeWorker *self = reinterpret_cast<PurgeWorker *>(privdata);	
	printf("onkeydata\n");
	//self->purgeMatchingKeys(response);  
}

void PurgeWorker::onConnect(const redisAsyncContext* redis, int status) {
	if (status == REDIS_OK) {
		printf("connected to redis, listening for product_ids\n");	
	} else {
  		printf("ERROR: %s\n", redis->errstr);	
	}
}

void PurgeWorker::onDisconnect(const redisAsyncContext* redis, int status) {
	if (status != REDIS_OK) {
		printf("ERROR: %s\n", redis->errstr);
		return;
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
