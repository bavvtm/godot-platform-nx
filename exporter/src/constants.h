#pragma once

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

static constexpr const char *PLATFORM_NAME = "Nintendo Switch";
static constexpr const char *PLATFORM_OS_NAME = "Switch";
static constexpr const char *BINARY_EXT = "nro";

// dirs 
static constexpr const char *ADDON_ROOT_DIR = "res://addons/switch_exporter";
static constexpr const char *ROMFS_PATH = "res://addons/switch_exporter/romfs";
static constexpr const char *BIN_DIR = "res://addons/switch_exporter/bin";
static constexpr const char *DEFAULT_TEMPLATE_PATH = "res://addons/switch_exporter/template_release";
static constexpr const char *DEFAULT_ICON_PATH = "res://addons/switch_exporter/default_icon.jpg";

// toolchain
static inline String find_dkp_tool(const String &p_tool_name) {
	OS *os = OS::get_singleton();
	bool is_win = os->get_name() == "Windows";
	String exe_ext = is_win ? ".exe" : "";
	String os_sub = is_win ? "windows" : "linux";

	String bundled = ProjectSettings::get_singleton()->globalize_path(
			String(BIN_DIR).path_join(os_sub).path_join(p_tool_name + exe_ext));
	if (FileAccess::file_exists(bundled)) {
		return bundled;
	}

	String dkp = os->get_environment("DEVKITPRO");
	if (dkp.is_empty() && !is_win) {
		dkp = "/opt/devkitpro";
	}
	if (dkp.is_empty()) {
		return "";
	}

	String path_bin = dkp.path_join("bin/" + p_tool_name + exe_ext);
	if (FileAccess::file_exists(path_bin)) {
		return path_bin;
	}

	String path_tools = dkp.path_join("tools/bin/" + p_tool_name + exe_ext);
	if (FileAccess::file_exists(path_tools)) {
		return path_tools;
	}

	String path_dk64 = dkp.path_join("devkitA64/bin/" + p_tool_name + exe_ext);
	if (FileAccess::file_exists(path_dk64)) {
		return path_dk64;
	}

	return "";
}
