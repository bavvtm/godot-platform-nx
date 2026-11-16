#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

// thirdparty

struct mbedtls_timing_delay_context {
	uint64_t timer_ms;
	uint32_t int_ms;
	uint32_t fin_ms;
};

int64_t mbedtls_ms_time(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (int64_t)ts.tv_sec * 1000LL + (int64_t)(ts.tv_nsec / 1000000LL);
}

void mbedtls_timing_set_delay(void *p_data, uint32_t p_int_ms, uint32_t p_fin_ms) {
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *)p_data;
	ctx->int_ms = p_int_ms;
	ctx->fin_ms = p_fin_ms;
	if (p_fin_ms != 0) {
		ctx->timer_ms = (uint64_t)mbedtls_ms_time();
	}
}

int mbedtls_timing_get_delay(void *p_data) {
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *)p_data;
	if (ctx->fin_ms == 0) {
		return -1;
	}
	uint64_t elapsed_ms = (uint64_t)mbedtls_ms_time() - ctx->timer_ms;
	if (elapsed_ms >= ctx->fin_ms) {
		return 2;
	}
	if (elapsed_ms >= ctx->int_ms) {
		return 1;
	}
	return 0;
}

// sys

int execvp(const char *, char *const[]) {
	errno = ENOSYS;
	return -1;
}

int mkfifo(const char *, mode_t) {
	errno = ENOSYS;
	return -1;
}

int pclose(FILE *) {
	errno = ENOSYS;
	return -1;
}

int pipe(int[2]) {
	errno = ENOSYS;
	return -1;
}

FILE *popen(const char *, const char *) {
	errno = ENOSYS;
	return nullptr;
}

pid_t setsid(void) {
	errno = ENOSYS;
	return -1;
}

int sigaction(int, const struct sigaction *, struct sigaction *) {
	errno = ENOSYS;
	return -1;
}

pid_t waitpid(pid_t, int *, int) {
	errno = ENOSYS;
	return -1;
}

#ifdef __cplusplus
}
#endif
