#include "switch_wrap.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

namespace nx {

FILE *s_log_file = nullptr;

static int _log_write_cb(void *, const char *p_buf, size_t p_len) {
	if (s_log_file && p_len > 0) {
		fwrite(p_buf, 1, p_len, s_log_file);
		fflush(s_log_file);
	}
	return (int)p_len;
}

void ensure_dir(const char *p_path) {
	if (!p_path || p_path[0] == '\0') {
		return;
	}

	char tmp[512];
	snprintf(tmp, sizeof(tmp), "%s", p_path);
	for (char *p = tmp + 1; *p; p++) {
		if (*p == '/') {
			*p = '\0';
			if (p[-1] != ':') {
				mkdir(tmp, 0777);
			}
			*p = '/';
		}
	}
	mkdir(tmp, 0777);
}

void init_file_logging_from_argv(const char *p_argv0) {
	char app_dir[512] = "sdmc:/switch";

	if (p_argv0 && p_argv0[0] != '\0') {
		if (strncmp(p_argv0, "sdmc:/", 6) == 0) {
			snprintf(app_dir, sizeof(app_dir), "%s", p_argv0);
		} else if (p_argv0[0] == '/') {
			snprintf(app_dir, sizeof(app_dir), "sdmc:%s", p_argv0);
		} else {
			snprintf(app_dir, sizeof(app_dir), "sdmc:/switch/%s", p_argv0);
		}

		char *last_slash = strrchr(app_dir, '/');
		if (last_slash && last_slash > app_dir + 5) {
			*last_slash = '\0';
		}
	}

	ensure_dir("sdmc:/switch");
	ensure_dir(app_dir);

	char log_path[544];
	snprintf(log_path, sizeof(log_path), "%s/logs.txt", app_dir);
	s_log_file = fopen(log_path, "w");
	if (s_log_file) {
		FILE *stream = funopen(nullptr, nullptr, _log_write_cb, nullptr, nullptr);
		if (stream) {
			stdout = stream;
			stderr = stream;
			setvbuf(stdout, nullptr, _IONBF, 0);
			setvbuf(stderr, nullptr, _IONBF, 0);
		}
	}
}

u32 hardware_type() {
	static u32 cached_type = 0xFFFFFFFF;
	if (cached_type != 0xFFFFFFFF) {
		return cached_type;
	}

	if (R_FAILED(splInitialize())) {
		return 0xFFFFFFFF;
	}

	u64 type = 0;
	Result rc = splGetConfig(SplConfigItem_HardwareType, &type);
	splExit();

	if (R_FAILED(rc)) {
		return 0xFFFFFFFF;
	}

	cached_type = static_cast<u32>(type);
	return cached_type;
}

bool firmware_version(char *r_buf, size_t p_len) {
	if (!r_buf || p_len == 0) {
		return false;
	}

	if (R_FAILED(setsysInitialize())) {
		return false;
	}

	SetSysFirmwareVersion fw;
	Result rc = setsysGetFirmwareVersion(&fw);
	setsysExit();

	if (R_FAILED(rc)) {
		return false;
	}

	strncpy(r_buf, fw.display_version, p_len - 1);
	r_buf[p_len - 1] = '\0';
	return true;
}

bool is_oled_model() {
	return hardware_type() == HARDWARE_AULA;
}

bool is_lite_model() {
	return hardware_type() == HARDWARE_HOAG;
}

} // namespace nx

