#ifndef fastpurge_varnish_cfg_h
#define fastpurge_varnish_cfg_h 1

#include <string>

struct varnish_cfg {
	std::string host;
	int port;
};

#endif
