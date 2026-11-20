#include "nro_builder.h"
#include "constants.h"

#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Error NroBuilder::_convert_icon_to_jpeg(const String &p_src_icon_path, const String &p_out_jpg_path) {
	const int ICON_SIZE = 256;
	ProjectSettings *ps = ProjectSettings::get_singleton();

	Ref<Image> src_img;
	src_img.instantiate();

	String resolved_icon = p_src_icon_path;
	if (resolved_icon.is_empty() || !FileAccess::file_exists(resolved_icon)) {
		resolved_icon = ps->get_setting("application/config/icon", "");
	}

	if (!resolved_icon.is_empty() && FileAccess::file_exists(resolved_icon)) {
		src_img->load(ps->globalize_path(resolved_icon));
	} else if (FileAccess::file_exists(DEFAULT_ICON_PATH)) {
		src_img->load(ps->globalize_path(DEFAULT_ICON_PATH));
	}

	if (src_img->is_empty()) {
		return ERR_CANT_OPEN;
	}

	src_img->decompress();
	src_img->convert(Image::FORMAT_RGBA8);
	src_img->resize(ICON_SIZE, ICON_SIZE, Image::INTERPOLATE_LANCZOS);

	Ref<Image> bg_img = Image::create_empty(ICON_SIZE, ICON_SIZE, false, Image::FORMAT_RGBA8);
	bg_img->fill(Color(0.0f, 0.0f, 0.0f, 1.0f));
	bg_img->blend_rect(src_img, Rect2i(0, 0, ICON_SIZE, ICON_SIZE), Vector2i(0, 0));
	bg_img->convert(Image::FORMAT_RGB8);

	return bg_img->save_jpg(p_out_jpg_path, 0.92f);
}

Error NroBuilder::_prepare_staging_romfs(const String &p_src_romfs_dir, const String &p_staging_romfs_dir, const String &p_fused_pck_path) {
	DirAccess::make_dir_recursive_absolute(p_staging_romfs_dir);

	String target_pck = p_staging_romfs_dir.path_join("game.pck");
	DirAccess::remove_absolute(target_pck);

	String splash_src = p_src_romfs_dir.path_join("applet_splash.rgba.z");
	if (FileAccess::file_exists(splash_src)) {
		DirAccess::copy_absolute(splash_src, p_staging_romfs_dir.path_join("applet_splash.rgba.z"));
	}

	if (!p_fused_pck_path.is_empty()) {
		Error copy_err = DirAccess::copy_absolute(p_fused_pck_path, target_pck);
		if (copy_err != OK) {
			UtilityFunctions::push_error("failed to copy fused PCK to staging romfs: ", p_fused_pck_path);
			return copy_err;
		}
	}

	return OK;
}

Error NroBuilder::_generate_nacp(const String &p_nacptool_bin, const NroBuildConfig &p_config, const String &p_out_nacp_path) {
	PackedStringArray args;
	args.push_back("--create");
	args.push_back(p_config.app_title);
	args.push_back(p_config.app_author);
	args.push_back(p_config.app_version);
	args.push_back(p_out_nacp_path);

	Array output;
	int exit_code = OS::get_singleton()->execute(p_nacptool_bin, args, output, true);
	if (exit_code != 0) {
		UtilityFunctions::push_error("'nacptool' failed: ", output);
		return FAILED;
	}

	return OK;
}

Error NroBuilder::_generate_nro(const String &p_elf2nro_bin, const NroBuildConfig &p_config, const String &p_nacp_path, const String &p_romfs_dir, const String &p_icon_jpg_path) {
	PackedStringArray args;
	args.push_back(p_config.template_elf_path);
	args.push_back(output_path);
	args.push_back("--nacp=" + p_nacp_path);
	args.push_back("--romfsdir=" + p_romfs_dir);

	if (!p_icon_jpg_path.is_empty() && FileAccess::file_exists(p_icon_jpg_path)) {
		args.push_back("--icon=" + p_icon_jpg_path);
	}

	Array output;
	int exit_code = OS::get_singleton()->execute(p_elf2nro_bin, args, output, true);
	if (exit_code != 0) {
		UtilityFunctions::push_error("'elf2nro' failed: ", output);
		return FAILED;
	}

	return OK;
}

Error NroBuilder::build(const NroBuildConfig &p_config) {
	String nacptool = find_dkp_tool("nacptool");
	String elf2nro = find_dkp_tool("elf2nro");
	if (nacptool.is_empty() || elf2nro.is_empty()) {
		UtilityFunctions::push_error("'nacptool' or 'elf2nro' not found in addon bin/ or DEVKITPRO.");
		return ERR_UNCONFIGURED;
	}

	if (output_path.is_empty()) {
		UtilityFunctions::push_error("output path is not set on NroBuilder.");
		return ERR_INVALID_PARAMETER;
	}

	ProjectSettings *ps = ProjectSettings::get_singleton();
	String tmp_dir = ps->globalize_path("res://.godot/switch_export_tmp");
	String romfs_dir = tmp_dir.path_join("romfs");
	String nacp_path = tmp_dir.path_join("control.nacp");
	String icon_jpg = tmp_dir.path_join("icon_converted.jpg");

	String src_romfs = p_config.romfs_source_dir.is_empty() ? ps->globalize_path(ROMFS_PATH) : p_config.romfs_source_dir;

	Error staging_err = _prepare_staging_romfs(src_romfs, romfs_dir, p_config.fused_pck_path);
	if (staging_err != OK) {
		return staging_err;
	}

	Error nacp_err = _generate_nacp(nacptool, p_config, nacp_path);
	if (nacp_err != OK) {
		return nacp_err;
	}

	bool has_icon = (_convert_icon_to_jpeg(p_config.icon_source_path, icon_jpg) == OK);
	return _generate_nro(elf2nro, p_config, nacp_path, romfs_dir, has_icon ? icon_jpg : "");
}

NroBuilder::NroBuilder() {
	// Does nothing.
}

NroBuilder::~NroBuilder() {
	// Does nothing.
}
