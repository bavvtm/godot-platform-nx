#include "editor_export_platform_switch.h"

#if __has_include("logo_data.gen.h")
#include "logo_data.gen.h"
#define HAS_GENERATED_LOGO 1
#endif

#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void EditorExportPlatformSwitch::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_pck_add_file", "path", "data", "file_index", "total_files", "enc_include_filters", "enc_exclude_filters", "key"), &EditorExportPlatformSwitch::_pck_add_file);
}

void EditorExportPlatformSwitch::_init_logos() {
#ifdef HAS_GENERATED_LOGO
	PackedByteArray data;
	data.resize(sizeof(switch_logo_rgba));
	memcpy(data.ptrw(), switch_logo_rgba, sizeof(switch_logo_rgba));

	Ref<Image> img = Image::create_from_data(SWITCH_LOGO_SIZE, SWITCH_LOGO_SIZE, false, Image::FORMAT_RGBA8, data);
	logo = ImageTexture::create_from_image(img);

	Ref<Image> run_img = img->duplicate();
	run_img->resize(16, 16, Image::INTERPOLATE_LANCZOS);
	run_icon = ImageTexture::create_from_image(run_img);
#else
	Ref<Image> logo_img = Image::create_empty(32, 32, false, Image::FORMAT_RGBA8);
	logo_img->fill(Color(0.90f, 0.00f, 0.07f, 1.0f));
	logo = ImageTexture::create_from_image(logo_img);

	Ref<Image> run_img = Image::create_empty(16, 16, false, Image::FORMAT_RGBA8);
	run_img->fill(Color(0.90f, 0.00f, 0.07f, 1.0f));
	run_icon = ImageTexture::create_from_image(run_img);
#endif
}

Error EditorExportPlatformSwitch::_pck_add_file(const String &p_path, const PackedByteArray &p_data, int32_t p_file_index, int32_t p_total_files, const PackedStringArray &p_enc_include_filters, const PackedStringArray &p_enc_exclude_filters, const PackedByteArray &p_key) {
	if (active_packer.is_null()) {
		return ERR_UNCONFIGURED;
	}

	if (p_total_files > 0) {
		float ratio = (float)(p_file_index + 1) / (float)p_total_files;
	}

	if (p_path.begins_with(String(ADDON_ROOT_DIR) + "/")) {
		return OK;
	}

	PackedByteArray final_data = p_data;
	if (p_path == "res://.godot/extension_list.cfg") {
		String cfg_text = String::utf8((const char *)p_data.ptr(), p_data.size());
		PackedStringArray lines = cfg_text.split("\n", false);
		PackedStringArray filtered_lines;
		for (int i = 0; i < lines.size(); i++) {
			if (!lines[i].contains("switch_exporter.gdextension")) {
				filtered_lines.push_back(lines[i]);
			}
		}
		if (filtered_lines.is_empty()) {
			return OK;
		}
		final_data = (String("\n").join(filtered_lines) + "\n").to_utf8_buffer();
	}

	String tmp_file = ProjectSettings::get_singleton()->globalize_path("res://.godot/switch_pck_entry.tmp");
	Ref<FileAccess> f = FileAccess::open(tmp_file, FileAccess::WRITE);
	if (f.is_null()) {
		return ERR_FILE_CANT_WRITE;
	}
	f->store_buffer(final_data);
	f.unref();

	Error err = active_packer->add_file(p_path, tmp_file);
	DirAccess::remove_absolute(tmp_file);
	return err;
}

Error EditorExportPlatformSwitch::_build_pck(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_pck_path) {
	active_packer.instantiate();
	Error start_err = active_packer->pck_start(p_pck_path);
	if (start_err != OK) {
		active_packer.unref();
		return start_err;
	}

	Error export_err = export_project_files(p_preset, p_debug, Callable(this, "_pck_add_file"));
	Error flush_err = active_packer->flush(true);
	active_packer.unref();

	return (export_err != OK) ? export_err : flush_err;
}

PackedStringArray EditorExportPlatformSwitch::_get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const {
	PackedStringArray list;
	list.push_back(BINARY_EXT);
	return list;
}

PackedStringArray EditorExportPlatformSwitch::_get_platform_features() const {
	PackedStringArray features;
	features.push_back("switch");
	features.push_back("console");
	features.push_back("arm64");
	return features;
}

PackedStringArray EditorExportPlatformSwitch::_get_preset_features(const Ref<EditorExportPreset> &p_preset) const {
	PackedStringArray features;
	features.push_back("switch");
	features.push_back("arm64");
	features.push_back("s3tc");
	return features;
}

TypedArray<Dictionary> EditorExportPlatformSwitch::_get_export_options() const {
	TypedArray<Dictionary> options;
	ProjectSettings *ps = ProjectSettings::get_singleton();

	String default_title = ps->get_setting("application/config/name", "Godot Switch Game");
	if (default_title.is_empty()) {
		default_title = "Godot Switch Game";
	}

	String default_version = ps->get_setting("application/config/version", "1.0.0");
	if (default_version.is_empty()) {
		default_version = "1.0.0";
	}

	String default_icon = ps->get_setting("application/config/icon", "");
	if (default_icon.is_empty() || !FileAccess::file_exists(default_icon)) {
		default_icon = DEFAULT_ICON_PATH;
	}

	Dictionary opt_template;
	opt_template["name"] = "custom_template/release";
	opt_template["type"] = Variant::STRING;
	opt_template["hint"] = PROPERTY_HINT_FILE;
	opt_template["hint_string"] = "*";
	opt_template["default_value"] = DEFAULT_TEMPLATE_PATH;
	options.push_back(opt_template);

	Dictionary opt_fused;
	opt_fused["name"] = "binary_format/fused_build";
	opt_fused["type"] = Variant::BOOL;
	opt_fused["default_value"] = false;
	options.push_back(opt_fused);

	Dictionary opt_title;
	opt_title["name"] = "application/title";
	opt_title["type"] = Variant::STRING;
	opt_title["default_value"] = default_title;
	options.push_back(opt_title);

	Dictionary opt_author;
	opt_author["name"] = "application/author";
	opt_author["type"] = Variant::STRING;
	opt_author["default_value"] = "Homebrew Developer";
	options.push_back(opt_author);

	Dictionary opt_version;
	opt_version["name"] = "application/version";
	opt_version["type"] = Variant::STRING;
	opt_version["default_value"] = default_version;
	options.push_back(opt_version);

	Dictionary opt_icon;
	opt_icon["name"] = "application/icon";
	opt_icon["type"] = Variant::STRING;
	opt_icon["hint"] = PROPERTY_HINT_FILE;
	opt_icon["hint_string"] = "*.png,*.jpg,*.jpeg,*.webp,*.svg";
	opt_icon["default_value"] = default_icon;
	options.push_back(opt_icon);

	return options;
}

bool EditorExportPlatformSwitch::_has_valid_export_configuration(const Ref<EditorExportPreset> &p_preset, bool p_debug) const {
	String err;
	bool valid = true;

	String template_path = p_preset->get("custom_template/release");
	if (template_path.is_empty() || !FileAccess::file_exists(template_path)) {
		err += "release template ELF not found at: " + template_path + "\n";
		valid = false;
	}

	if (find_dkp_tool("nacptool").is_empty() || find_dkp_tool("elf2nro").is_empty()) {
		err += "'nacptool' or 'elf2nro' not found in addon bin/ or DEVKITPRO.\n";
		valid = false;
	}

	if (!err.is_empty()) {
		set_config_error(err.strip_edges());
	}
	return valid;
}

bool EditorExportPlatformSwitch::_poll_export() {
	return discovery ? discovery->consume_devices_changed() : false;
}

int32_t EditorExportPlatformSwitch::_get_options_count() const {
	return discovery ? discovery->get_device_count() : 0;
}

String EditorExportPlatformSwitch::_get_options_tooltip() const {
	return "Select Nintendo Switch device from the local network";
}

String EditorExportPlatformSwitch::_get_option_label(int32_t p_device) const {
	return discovery ? discovery->get_device_ip(p_device) : String();
}

String EditorExportPlatformSwitch::_get_option_tooltip(int32_t p_device) const {
	return _get_option_label(p_device);
}

Error EditorExportPlatformSwitch::_run(const Ref<EditorExportPreset> &p_preset, int32_t p_device, BitField<EditorExportPlatform::DebugFlags> p_debug_flags) {
	if (!discovery) {
		return ERR_UNCONFIGURED;
	}

	String tmp_pck = ProjectSettings::get_singleton()->globalize_path("res://.godot/tmpexport.pck");
	Error pack_err = _build_pck(p_preset, true, tmp_pck);
	if (pack_err != OK) {
		DirAccess::remove_absolute(tmp_pck);
		return pack_err;
	}

	Error deploy_err = discovery->deploy_pck(p_device, tmp_pck);
	DirAccess::remove_absolute(tmp_pck);
	return deploy_err;
}

Error EditorExportPlatformSwitch::_export_pack(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, BitField<EditorExportPlatform::DebugFlags> p_flags) {
	Error err = _build_pck(p_preset, p_debug, p_path);
	return err;
}

Error EditorExportPlatformSwitch::_export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, BitField<EditorExportPlatform::DebugFlags> p_flags) {
	ProjectSettings *ps = ProjectSettings::get_singleton();
	String out_nro_abs = ps->globalize_path(p_path);

	String app_title = p_preset->get("application/title");
	if (app_title.is_empty()) {
		app_title = ps->get_setting("application/config/name", out_nro_abs.get_file().get_basename());
	}

	NroBuildConfig config;
	config.app_title = app_title;
	config.app_author = p_preset->get("application/author");
	config.app_version = p_preset->get("application/version");
	config.icon_source_path = p_preset->get("application/icon");
	config.romfs_source_dir = ps->globalize_path(ROMFS_PATH);
	config.template_elf_path = ps->globalize_path(p_preset->get("custom_template/release"));

	bool is_fused = p_preset->get("binary_format/fused_build");
	if (is_fused) {
		String tmp_pck_abs = ps->globalize_path("res://.godot/switch_export_tmp/game.pck");
		DirAccess::make_dir_recursive_absolute(tmp_pck_abs.get_base_dir());

		Error pack_err = _build_pck(p_preset, p_debug, tmp_pck_abs);
		if (pack_err != OK) {
			UtilityFunctions::push_error("failed to pack fused PCK file at ", tmp_pck_abs);
			return pack_err;
		}

		config.fused_pck_path = tmp_pck_abs;
		nro_builder.set_output_path(out_nro_abs);
		Error nro_err = nro_builder.build(config);
		DirAccess::remove_absolute(tmp_pck_abs);

		if (nro_err != OK) {
			return nro_err;
		}

		UtilityFunctions::print("successfully exported fused NRO: ", out_nro_abs);
		return OK;
	}

	nro_builder.set_output_path(out_nro_abs);
	Error nro_err = nro_builder.build(config);
	if (nro_err != OK) {
		return nro_err;
	}

	String out_pck_abs = out_nro_abs.get_basename() + ".pck";
	Error pack_err = _build_pck(p_preset, p_debug, out_pck_abs);
	if (pack_err != OK) {
		UtilityFunctions::push_error("failed to pack PCK file at ", out_pck_abs);
		return pack_err;
	}

	UtilityFunctions::print("successfully exported ", out_nro_abs, " and ", out_pck_abs);
	return OK;
}

EditorExportPlatformSwitch::EditorExportPlatformSwitch() {
	_init_logos();

	discovery = memnew(DeviceDiscoverySwitch);
	discovery->start();
}

EditorExportPlatformSwitch::~EditorExportPlatformSwitch() {
	if (discovery) {
		discovery->stop();
		memdelete(discovery);
		discovery = nullptr;
	}
}
