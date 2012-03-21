#ifndef fastpurge_redis_cfg_h
#define fastpurge_refis_cfg_h 1

	struct redis_cfg {
		char host[250];
		char skey[1024];
		int port;
	};

#endif
