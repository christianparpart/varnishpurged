#ifndef fastpurge_redis_cfg_h
#define fastpurge_refis_cfg_h 1

#include <string>

struct redis_cfg {
	std::string host;
	std::string skey;
	int port;
};

#endif
