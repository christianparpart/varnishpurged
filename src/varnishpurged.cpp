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

void parseAddress(char* address_, char* host, int* port){
	std::string address = address_;
	int ind = address.find(":");

	if(ind == -1){
		printf("ERROR: invalid address: %s\n", address_);
		exit(1);
	}

	*port = atoi(address.substr(ind+1).c_str());   
	// strncpy(host, address.substr(0, ind).c_str(), STR_BUFSIZE);
}


int main(int argc, char* argv[]) {
	ev::default_loop ev;

	if(argc != 3){
		printf("usage: %s varnish_host:port redis_host:port\n", argv[0]);
		exit(1);
	}


	varnish_cfg* varnish_config = (varnish_cfg *)malloc(sizeof(varnish_cfg));
	parseAddress(argv[1], varnish_config->host, &varnish_config->port);

	redis_cfg* redis_config = (redis_cfg *)malloc(sizeof(redis_cfg));
	strncpy(redis_config->skey, "fnord:queue", sizeof(redis_config->host));
	parseAddress(argv[2], redis_config->host, &redis_config->port);


	PurgeWorker* worker = new PurgeWorker(ev, redis_config, varnish_config);

	ev_loop(ev, 0);
	return 0;
}