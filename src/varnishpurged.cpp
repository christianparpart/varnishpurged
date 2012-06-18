#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <ev++.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

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

bool dropPrivileges(const std::string& username, const std::string& groupname)
{
	if (!groupname.empty() && !getgid()) {
		if (struct group *gr = getgrnam(groupname.c_str())) {
			if (setgid(gr->gr_gid) != 0) {
				fprintf(stderr, "could not setgid to %s: %s\n", groupname.c_str(), strerror(errno));
				return false;
			}

			setgroups(0, nullptr);

			if (!username.empty()) {
				initgroups(username.c_str(), gr->gr_gid);
			}
		} else {
			fprintf(stderr, "Could not find group: %s\n", groupname.c_str());
			return false;
		}
	}

	if (!username.empty() && !getuid()) {
		if (struct passwd *pw = getpwnam(username.c_str())) {
			if (setuid(pw->pw_uid) != 0) {
				fprintf(stderr, "could not setgid to %s: %s\n", username.c_str(), strerror(errno));
				return false;
			}
		} else {
			fprintf(stderr, "Could not find group: %s\n", groupname.c_str());
			return false;
		}
	}

	if (!::getuid() || !::geteuid() || !::getgid() || !::getegid()) {
		fprintf(stderr, "Service is not allowed to run with administrative permissionsService is still running with administrative permissions.");
		return false;
	}

	return true;
}

void printHelp()
{
	printf("varnishpurged [-d] [-u USER] [-g GROUP] -v VARNISH:PORT -r REDIS:PORT\n");
}

int main(int argc, char * const argv[])
{
	struct option long_options[] = {
		{ "daemon", no_argument, nullptr, 'd' },
		{ "user", required_argument, nullptr, 'u' },
		{ "group", required_argument, nullptr, 'g' },
		{ "varnish", required_argument, nullptr, 'v' },
		{ "redis", required_argument, nullptr, 'r' },
		{ 0, 0, 0, 0 }
	};

	bool daemonize = false;
	std::string userName;
	std::string groupName;
	std::string varnishAddr;
	std::string redisAddr;

	for (bool done = false; !done; ) {
		int long_index = 0;
		switch (getopt_long(argc, argv, "hdu:g:v:r:", long_options, &long_index)) {
			case 'h':
				printHelp();
				return 0;
			case 'd':
				daemonize = true;
				break;
			case 'u':
				userName = optarg;
				break;
			case 'g':
				groupName = optarg;
				break;
			case 'v':
				varnishAddr = optarg;
				break;
			case 'r':
				redisAddr = optarg;
				break;
			case 0: // long opt with value != NULL
				done = true;
				break;
			case '?': // ambiguous match / unknown arg
				printHelp();
				return 1;
			default:
				done = true;
				break;
		}
	}

	if (varnishAddr.empty() || redisAddr.empty()) {
		printf("varnish: %s\n", varnishAddr.c_str());
		printf("redis: %s\n", redisAddr.c_str());
		printHelp();
		return 2;
	}

	if (!dropPrivileges(userName, groupName))
		return 3;

	ev::default_loop ev;

	auto varnish_config = std::make_shared<varnish_cfg>();
	if (!parseAddress(varnishAddr, varnish_config->host, varnish_config->port))
		return 4;

	auto redis_config = std::make_shared<redis_cfg>();
	redis_config->skey = REDIS_QUEUE_KEY;
	if (!parseAddress(redisAddr, redis_config->host, redis_config->port))
		return 5;

	std::unique_ptr<PurgeWorker> worker(new PurgeWorker(ev, redis_config.get(), varnish_config.get()));

	if (daemonize && ::daemon(false /*no chdir?*/, false /*no close?*/) < 0) {
		fprintf(stderr, "Daemonizing failed: %s\n", strerror(errno));
		return 6;
	}

	worker->run();

	return 0;
}
