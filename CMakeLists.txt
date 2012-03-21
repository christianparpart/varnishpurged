cmake_minimum_required(VERSION 2.8)

project(varnishpurged)
set(PACKAGE_NAME "varnishpurged")
set(PACKAGE_VERSION "0.1.0")
set(PACKAGE_HOMEPAGE_URL "http://github.com/dawanda/varnishpurged")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FindPkgConfig)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckVariableExists)
include(CheckTypeSize)
include(CheckLibraryExists)
include(CheckCSourceCompiles)
include(CMakeDetermineCCompiler)

add_definitions(-Wall -Wno-variadic-macros)

# we need the following definitions in order to get some special
# OS-level features like posix_fadvise() or readahead().
add_definitions(-DXOPEN_SOURCE=600)
add_definitions(-DGNU_SOURCE)

# enforce 64bit I/O operations, even on 32bit platforms
add_definitions(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGE_FILES)

# some compile-time requirements
CHECK_INCLUDE_FILES(regex.h HAVE_REGEX_H)
CHECK_INCLUDE_FILES(fcntl.h HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(netdb.h HAVE_NETDB_H)
CHECK_INCLUDE_FILES(sys/resource.h HAVE_SYS_RESOURCE_H)

CHECK_FUNCTION_EXISTS(fork HAVE_FORK)

# ---------------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# require ev library
link_libraries("ev")

# require hiredis
link_libraries("hiredis")

# require libcurl
link_libraries("curl")


# {{{ path prefixes
# ---------------------------------------------------------------------------
if (DEFINED CMAKE_INSTALL_PREFIX)
	set(SYSCONFDIR ${CMAKE_INSTALL_PREFIX}/etc CACHE PATH "configuration directory")
	set(INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include CACHE PATH "include directory")
	set(LIBDIR ${CMAKE_INSTALL_PREFIX}/lib CACHE PATH "library directory")
	set(LOGDIR ${CMAKE_INSTALL_PREFIX}/var/log CACHE PATH "log file directory")
endif()

if (DEFINED SYSCONFDIR)
	set(SYSCONFDIR ${SYSCONFDIR} CACHE PATH "configuration directory")
else()
	set(SYSCONFDIR ${CMAKE_INSTALL_PREFIX}/etc CACHE PATH "configuration directory")
endif()

if (DEFINED INCLUDEDIR)
	set(INCLUDEDIR ${INCLUDEDIR} CACHE PATH "path to library includes")
else()
	set(INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include CACHE PATH "path to library includes")
endif()

if (DEFINED LIBDIR)
	set(LIBDIR ${LIBDIR} CACHE PATH "path to program libraries")
else()
	set(LIBDIR ${CMAKE_INSTALL_PREFIX}/lib CACHE PATH "path to program libraries")
endif()

if (DEFINED LOGDIR)
	set(LOGDIR ${LOGDIR} CACHE PATH "log file directory")
else()
	set(LOGDIR ${CMAKE_INSTALL_PREFIX}/var/log CACHE PATH "log file directory")
endif()
# ---------------------------------------------------------------------------
# }}}

add_subdirectory(src)
