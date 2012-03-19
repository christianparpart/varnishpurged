#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <cstring>
#include <vector>
#include <string>
#include <cstdlib>
#include <ev++.h>

#include "dwn_ppurge.h"


int main(int argc, char* argv[]) {
	ev::default_loop ev;

	PurgeWorker worker = new PurgeWorker();

	ev_loop(ev, 0);
	return 0;
}

