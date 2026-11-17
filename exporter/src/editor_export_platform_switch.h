#pragma once

#include "constants.h"
#include "device_discovery_switch.h"
#include "nro_builder.h"

#include <godot_cpp/classes/editor_export_platform.hpp>
#include <godot_cpp/classes/editor_export_platform_extension.hpp>
#include <godot_cpp/classes/editor_export_preset.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/pck_packer.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;

class EditorExportPlatformSwitch : public EditorExportPlatformExtension {
	GDCLASS(EditorExportPlatformSwitch, EditorExportPlatformExtension);

private:
	Ref<ImageTexture> logo;
	Ref<ImageTexture> run_icon;

	DeviceDiscoverySwitch *discovery = nullptr;
	NroBuilder nro_builder;

	Ref<PCKPacker> active_packer;

	void _init_logos();

	Error _pck_add_file(const String &p_path, const PackedByteArray &p_data, int32_t p_file_index, int32_t p_total_files, const PackedStringArray &p_enc_include_filters, const PackedStringArray &p_enc_exclude_filters, const PackedByteArray &p_key);
	Error _build_pck(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_pck_path);

protected:
	static void _bind_methods();

public:
	String _get_name() const override { return PLATFORM_NAME; };
	String _get_os_name() const override { return PLATFORM_OS_NAME; };
	Ref<Texture2D> _get_logo() const override { return logo; }
	Ref<Texture2D> _get_run_icon() const override { return run_icon; }

	PackedStringArray _get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const override;
	PackedStringArray _get_platform_features() const override;
	PackedStringArray _get_preset_features(const Ref<EditorExportPreset> &p_preset) const override;
	TypedArray<Dictionary> _get_export_options() const override;

	bool _has_valid_export_configuration(const Ref<EditorExportPreset> &p_preset, bool p_debug) const override;
	bool _has_valid_project_configuration(const Ref<EditorExportPreset> &p_preset) const override { return true; };

	bool _poll_export() override;
	int32_t _get_options_count() const override;
	String _get_options_tooltip() const override;
	String _get_option_label(int32_t p_device) const override;
	String _get_option_tooltip(int32_t p_device) const override;

	Error _run(const Ref<EditorExportPreset> &p_preset, int32_t p_device, BitField<EditorExportPlatform::DebugFlags> p_debug_flags) override;
	Error _export_pack(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, BitField<EditorExportPlatform::DebugFlags> p_flags) override;
	Error _export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, BitField<EditorExportPlatform::DebugFlags> p_flags) override;

	EditorExportPlatformSwitch();
	~EditorExportPlatformSwitch() override;
};
