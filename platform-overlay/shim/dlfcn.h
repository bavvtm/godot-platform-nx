#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define RTLD_LAZY 0x00001
#define RTLD_NOW 0x00002
#define RTLD_GLOBAL 0x00100
#define RTLD_LOCAL 0x00000
#define RTLD_NEXT ((void *)-1L)
#define RTLD_DEFAULT ((void *)0)

static inline void *dlopen(const char *p_filename, int p_flags) {
	(void)p_filename;
	(void)p_flags;
	return NULL;
}

static inline void *dlsym(void *p_handle, const char *p_symbol) {
	(void)p_handle;
	(void)p_symbol;
	return NULL;
}

static inline int dlclose(void *p_handle) {
	(void)p_handle;
	return 0;
}

static inline char *dlerror(void) {
	return (char *)"Dynamic loading (dlopen) is not supported on Horizon OS";
}

#ifdef __cplusplus
}
#endif
