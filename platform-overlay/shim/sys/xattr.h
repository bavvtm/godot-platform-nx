#pragma once

#include <sys/types.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline ssize_t getxattr(const char *path, const char *name, void *value, size_t size) {
	(void)path;
	(void)name;
	(void)value;
	(void)size;
	errno = ENOTSUP;
	return -1;
}

static inline ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size) {
	(void)path;
	(void)name;
	(void)value;
	(void)size;
	errno = ENOTSUP;
	return -1;
}

static inline ssize_t fgetxattr(int fd, const char *name, void *value, size_t size) {
	(void)fd;
	(void)name;
	(void)value;
	(void)size;
	errno = ENOTSUP;
	return -1;
}

static inline int setxattr(const char *path, const char *name, const void *value, size_t size, int flags) {
	(void)path;
	(void)name;
	(void)value;
	(void)size;
	(void)flags;
	errno = ENOTSUP;
	return -1;
}

static inline int lsetxattr(const char *path, const char *name, const void *value, size_t size, int flags) {
	(void)path;
	(void)name;
	(void)value;
	(void)size;
	(void)flags;
	errno = ENOTSUP;
	return -1;
}

static inline int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags) {
	(void)fd;
	(void)name;
	(void)value;
	(void)size;
	(void)flags;
	errno = ENOTSUP;
	return -1;
}

static inline ssize_t listxattr(const char *path, char *list, size_t size) {
	(void)path;
	(void)list;
	(void)size;
	errno = ENOTSUP;
	return -1;
}

static inline ssize_t llistxattr(const char *path, char *list, size_t size) {
	(void)path;
	(void)list;
	(void)size;
	errno = ENOTSUP;
	return -1;
}

static inline ssize_t flistxattr(int fd, char *list, size_t size) {
	(void)fd;
	(void)list;
	(void)size;
	errno = ENOTSUP;
	return -1;
}

static inline int removexattr(const char *path, const char *name) {
	(void)path;
	(void)name;
	errno = ENOTSUP;
	return -1;
}

static inline int lremovexattr(const char *path, const char *name) {
	(void)path;
	(void)name;
	errno = ENOTSUP;
	return -1;
}

static inline int fremovexattr(int fd, const char *name) {
	(void)fd;
	(void)name;
	errno = ENOTSUP;
	return -1;
}

#ifdef __cplusplus
}
#endif
