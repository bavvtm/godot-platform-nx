#pragma once

#include <errno.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ifaddrs {
	struct ifaddrs *ifa_next;
	char *ifa_name;
	unsigned int ifa_flags;
	struct sockaddr *ifa_addr;
	struct sockaddr *ifa_netmask;
	union {
		struct sockaddr *ifu_broadaddr;
		struct sockaddr *ifu_dstaddr;
	} ifa_ifu;
#ifndef ifa_broadaddr
#define ifa_broadaddr ifa_ifu.ifu_broadaddr
#endif
#ifndef ifa_dstaddr
#define ifa_dstaddr ifa_ifu.ifu_dstaddr
#endif
	void *ifa_data;
};

static inline int getifaddrs(struct ifaddrs **r_ifap) {
	if (r_ifap) {
		*r_ifap = nullptr;
	}
	return 0;
}

static inline void freeifaddrs(struct ifaddrs *p_ifa) {
	(void)p_ifa;
}

#ifdef __cplusplus
}
#endif
