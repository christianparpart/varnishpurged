#! /bin/bash

if [ "$1" == "clean" ]; then
  make clean
	find . -name 'CMakeCache.txt' -print | xargs rm -vrf &>/dev/null
	find . -name 'CMakeFiles*' -print | xargs rm -vrf &>/dev/null
	find . -name 'Makefile' -exec rm -f {} \;
	rm -f cmake_install.cmake
else
	cmake "$(dirname $0)" \
		-DCMAKE_CXX_FLAGS_DEBUG="-O0 -ggdb3" \
		-DCMAKE_BUILD_TYPE="debug" \
		-DCMAKE_INSTALL_PREFIX="$HOME/local"
fi
