#ifndef fastpurge_varnish_cfg_h
#define fastpurge_varnish_cfg_h 1

	struct varnish_cfg {
		char host[STR_BUFSIZE];
		int port;
	};

#endif
