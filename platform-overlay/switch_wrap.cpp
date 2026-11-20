#include "switch_wrap.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

namespace nx {

static bool _has_arg(char *const *p_args, int p_count, const char *p_target) {
	for (int i = 0; i < p_count; i++) {
		if (p_args[i] && strcmp(p_args[i], p_target) == 0) {
			return true;
		}
	}

	return false;
}

FILE *s_log_file = nullptr;

static int _log_write_cb(void *, const char *p_buf, size_t p_len) {
	static bool line_start = true;
	static uint64_t t0 = nx::millis();

	if (s_log_file && p_len > 0) {
		for (size_t i = 0; i < p_len; i++) {
			if (line_start) {
				uint64_t ms = nx::millis() - t0;
				fprintf(s_log_file, "[%4llu.%03llu] ", (unsigned long long)(ms / 1000), (unsigned long long)(ms % 1000));
				line_start = false;
			}
			fputc(p_buf[i], s_log_file);
			if (p_buf[i] == '\n') {
				line_start = true;
			}
		}
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

void get_app_dir_from_argv(const char *p_argv0, char *r_dir, size_t p_len) {
	if (!r_dir || p_len == 0) {
		return;
	}

	snprintf(r_dir, p_len, "sdmc:/switch");

	if (p_argv0 && p_argv0[0] != '\0') {
		if (strncmp(p_argv0, "sdmc:/", 6) == 0) {
			snprintf(r_dir, p_len, "%s", p_argv0);
		} else if (p_argv0[0] == '/') {
			snprintf(r_dir, p_len, "sdmc:%s", p_argv0);
		} else {
			snprintf(r_dir, p_len, "sdmc:/switch/%s", p_argv0);
		}

		char *last_slash = strrchr(r_dir, '/');
		if (last_slash && last_slash > r_dir + 5) {
			*last_slash = '\0';
		}
	}
}

void init_file_logging_from_argv(const char *p_argv0) {
	char app_dir[512];
	get_app_dir_from_argv(p_argv0, app_dir, sizeof(app_dir));

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

static void _parse_args_file(const char *p_file_path, char ***r_list, int *r_count) {
	FILE *f = fopen(p_file_path, "r");
	if (!f) {
		return;
	}

	char tok[256];
	while (fscanf(f, "%255s", tok) == 1) {
		// skip comment
		if (tok[0] == '#') {
			int c;
			while ((c = fgetc(f)) != '\n' && c != EOF) {}
			continue;
		}

		*r_list = (char **)realloc(*r_list, sizeof(char *) * (*r_count + 1));
		(*r_list)[(*r_count)++] = strdup(tok);
	}

	fclose(f);
}

int load_args_from_file(const char *p_argv0, char ***r_out_argv) {
	if (!r_out_argv) {
		return 0;
	}

	char **list = nullptr;
	int count = 0;

	char app_dir[512];
	get_app_dir_from_argv(p_argv0, app_dir, sizeof(app_dir));

	char args_path[544];
	snprintf(args_path, sizeof(args_path), "%s/args.txt", app_dir);

	_parse_args_file(args_path, &list, &count);
	if (count == 0) {
		_parse_args_file("romfs:/args.txt", &list, &count);
	}

	*r_out_argv = list;
	return count;
}

static bool _has_external_pck(const char *p_argv0) {
	if (!p_argv0 || p_argv0[0] == '\0') {
		return false;
	}

	char app_dir[512];
	get_app_dir_from_argv(p_argv0, app_dir, sizeof(app_dir));

	const char *base_name = strrchr(p_argv0, '/');
	base_name = base_name ? (base_name + 1) : p_argv0;

	char pck_path[544];
	snprintf(pck_path, sizeof(pck_path), "%s/%s", app_dir, base_name);

	char *last_dot = strrchr(pck_path, '.');
	char *last_slash = strrchr(pck_path, '/');
	if (last_dot && (!last_slash || last_dot > last_slash)) {
		snprintf(last_dot, sizeof(pck_path) - (last_dot - pck_path), ".pck");
	} else {
		strncat(pck_path, ".pck", sizeof(pck_path) - strlen(pck_path) - 1);
	}

	return access(pck_path, F_OK) == 0;
}

int build_setup_args(int argc, char *argv[], char ***r_out_argv) {
	if (!r_out_argv) {
		return 0;
	}

	const char *arg0 = (argc > 0 && argv) ? argv[0] : nullptr;
	int cli_argc = (argc > 1) ? (argc - 1) : 0;
	char **cli_argv = (cli_argc > 0) ? (argv + 1) : nullptr;

	char **file_argv = nullptr;
	int file_argc = load_args_from_file(arg0, &file_argv);

	bool has_custom_pack = _has_arg(cli_argv, cli_argc, "--main-pack") || _has_arg(file_argv, file_argc, "--main-pack");
	bool use_fused_pck = (!has_custom_pack && !_has_external_pck(arg0) && access("romfs:/game.pck", F_OK) == 0);

	int total = cli_argc + file_argc + (use_fused_pck ? 2 : 0);
	if (total == 0) {
		*r_out_argv = nullptr;
		return 0;
	}

	char **out = (char **)malloc(sizeof(char *) * total);
	int idx = 0;

	for (int i = 0; i < cli_argc; i++) {
		out[idx++] = cli_argv[i];
	}
	for (int i = 0; i < file_argc; i++) {
		out[idx++] = file_argv[i];
	}
	if (use_fused_pck) {
		out[idx++] = strdup("--main-pack");
		out[idx++] = strdup("romfs:/game.pck");
	}

	free(file_argv);
	*r_out_argv = out;
	return total;
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
