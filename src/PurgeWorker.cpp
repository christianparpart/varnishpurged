#include "PurgeWorker.h"

PurgeWorker::PurgeWorker(ev::loop_ref& loop_, char const *address_) : BaseAdapter(loop_, address_) {
	ip_addr* addr = parseAddress(address_);
	
	if (addr->port == 0){
		addr->port = REDIS_DEFAULT_PORT;
	}

	this->redis = redisAsyncConnect(addr->host, addr->port);
	redisLibevAttach(this->loop, this->redis);

	redisAsyncSetConnectCallback(this->redis, FNORDCAST1 &RedisPurger::onConnect);    
	redisAsyncSetDisconnectCallback(this->redis, FNORDCAST1 &RedisPurger::onDisconnect);    

	if (redis->err) {		
		printf("ERROR: %s\n", this->redis->errstr);	
		exit(1);
	}
}


void PurgeWorker::onKeydata(redisAsyncContext *redis, void* response, void* privdata) {
	RedisPurger *self = reinterpret_cast<RedisPurger *>(privdata);	
	printf("onkeydata\n");
	//self->purgeMatchingKeys(response);  
}

void PurgeWorker::onConnect(const redisAsyncContext* redis, int status) {
	if (status != REDIS_OK) {
		printf("ERROR: %s\n", redis->errstr);
		return;
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

  return addr;
}