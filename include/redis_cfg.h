#ifndef fastpurge_redis_cfg_h
#define fastpurge_refis_cfg_h 1

	struct redis_cfg {
		char host[STR_BUFSIZE];
		char skey[STR_BUFSIZE];
		int port;
	};

#endif
