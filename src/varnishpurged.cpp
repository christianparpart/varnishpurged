#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <cstring>
#include <vector>
#include <string>
#include <cstdlib>
#include <ev++.h>

#include "varnishpurged.h"
#include "redis_cfg.h"
#include "PurgeWorker.h"

int main(int argc, char* argv[]) {
	ev::default_loop ev;

	// FIXPAUL: parse_longopts and build these config structs
	redis_cfg* redis_config = (redis_cfg *)malloc(sizeof(redis_cfg));
	strncpy(redis_config->host, "localhost", sizeof(redis_config->host));
	strncpy(redis_config->skey, "fnord:queue", sizeof(redis_config->host));
	redis_config->port = 6379;


	// FIXPAUL: parse_longopts and build these config structs
    //redis_cfg* redis_config = (redis_cfg *)malloc(sizeof(redis_cfg));


	PurgeWorker* worker = new PurgeWorker(ev, "localhost");

	ev_loop(ev, 0);
	return 0;
}