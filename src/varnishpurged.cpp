#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <cstring>
#include <vector>
#include <string>
#include <cstdlib>
#include <ev++.h>

#include "varnishpurged.h"
#include "PurgeWorker.h"

int main(int argc, char* argv[]) {
	ev::default_loop ev;

	// FIXPAUL: parse_longopts and build these config structs
	redis_cfg* redis_config = (redis_cfg *)malloc(sizeof(redis_cfg));
	strncpy(redis_config->host, "localhost", sizeof(redis_config->host));
	strncpy(redis_config->skey, "fnord:queue", sizeof(redis_config->host));
	redis_config->port = 6379;

	// FIXPAUL: parse_longopts and build these config structs
	varnish_cfg* varnish_config = (varnish_cfg *)malloc(sizeof(varnish_cfg));
	strncpy(varnish_config->host, "localhost", sizeof(redis_config->host));
	varnish_config->port = 80;

	PurgeWorker* worker = new PurgeWorker(ev, redis_config, varnish_config);

	ev_loop(ev, 0);
	return 0;
}


// void parseAddress(const char* address_) {
// 	std::string address = address_;
// 	ip_addr* addr = (ip_addr *)malloc(sizeof(ip_addr));
// 	int ind = address.find(":");

// 	if(ind == -1){
// 		addr->port = 0; 
// 		strncpy(addr->host, address.c_str(), sizeof(addr->host));   
// 	} else {
// 		addr->port = atoi(address.substr(ind+1).c_str());   
// 		strncpy(addr->host, address.substr(0, ind).c_str(), sizeof(addr->host));    
// 	}

// 	if (addr->port == 0){
// 		addr->port = REDIS_DEFAULT_PORT;
// 	}

// 	return addr;
// }
