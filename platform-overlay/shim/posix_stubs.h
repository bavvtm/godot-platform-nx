#pragma once

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// resource limits (<sys/resource.h>)
#ifndef RLIMIT_STACK
#define RLIMIT_STACK 3

struct rlimit {
	unsigned long long rlim_cur;
	unsigned long long rlim_max;
};

static inline int getrlimit(int p_resource, struct rlimit *r_limit) {
	(void)p_resource;
	if (r_limit) {
		r_limit->rlim_cur = 0;
		r_limit->rlim_max = 0;
	}
	return 0;
}
#endif

// network & ipv6 socket shims (<net/if.h>, <netinet/in.h>)
#ifndef IF_NAMESIZE
#define IF_NAMESIZE 16
#endif

static inline char *if_indextoname(unsigned int p_ifindex, char *p_ifname) {
	if (!p_ifname) {
		return NULL;
	}
	snprintf(p_ifname, IF_NAMESIZE, "nx%u", p_ifindex);
	return p_ifname;
}

static inline unsigned int if_nametoindex(const char *p_ifname) {
	(void)p_ifname;
	return 0;
}

#ifndef IPV6_JOIN_GROUP
#define IPV6_JOIN_GROUP 12
#endif

#ifndef IPV6_LEAVE_GROUP
#define IPV6_LEAVE_GROUP 13
#endif

#if !defined(HAVE_IPV6_MREQ) && !defined(__DEFINED_struct_ipv6_mreq)
struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned int ipv6mr_interface;
};
#endif

#ifdef __cplusplus
}
#endif

// libstdc++'s <bits/ctype_base.h> requires newlib's _S macro (0x08) to define
// ctype_base::space, while thirdparty/manifold uses _S as a variable name.
// Pre-including <locale> evaluates ctype_base while _S is still defined,
// after which we safely undefine _S for C++ translation units only.
#include <ctype.h>
#ifdef __cplusplus
#include <locale>
#ifdef _S
#undef _S
#endif
#endif
