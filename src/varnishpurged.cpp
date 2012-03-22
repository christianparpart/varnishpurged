#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <ev++.h>

#include "varnishpurged.h"
#include "PurgeWorker.h"
#include "redis_cfg.h"
#include "varnish_cfg.h"

bool parseAddress(const std::string& address, std::string& host, int& port)
{
	size_t ind = address.find(":");

	if (ind == std::string::npos) {
		printf("ERROR: invalid address: %s\n", address.c_str());
		return false;
	}

	port = std::atoi(address.substr(ind + 1).c_str());   
	host = address.substr(0, ind);

	return true;
}


int main(int argc, const char* argv[])
{
	ev::default_loop ev;

	if (argc != 3) {
		printf("usage: %s varnish_host:port redis_host:port\n", argv[0]);
		return 1;
	}

	auto varnish_config = std::make_shared<varnish_cfg>();
	if (!parseAddress(argv[1], varnish_config->host, varnish_config->port))
		return 2;

	auto redis_config = std::make_shared<redis_cfg>();
	redis_config->skey = REDIS_QUEUE_KEY;
	if (!parseAddress(argv[2], redis_config->host, redis_config->port))
		return 3;

	std::unique_ptr<PurgeWorker> worker(new PurgeWorker(ev, redis_config.get(), varnish_config.get()));

	worker->run();

	return 0;
}
