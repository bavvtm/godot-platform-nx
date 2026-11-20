#pragma once

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

#define LOG_PID 0x01
#define LOG_CONS 0x02
#define LOG_NDELAY 0x08
#define LOG_USER (1 << 3)

static inline void openlog(const char *p_ident, int p_option, int p_facility) {
	(void)p_ident;
	(void)p_option;
	(void)p_facility;
}

static inline void syslog(int p_priority, const char *p_format, ...) {
	(void)p_priority;
	(void)p_format;
}

static inline void vsyslog(int p_priority, const char *p_format, va_list p_args) {
	(void)p_priority;
	(void)p_format;
	(void)p_args;
}

static inline void closelog(void) {}

#ifdef __cplusplus
}
#endif