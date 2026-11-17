#pragma once

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

struct NroBuildConfig {
	String app_title;
	String app_author;
	String app_version;
	String icon_source_path;
  String romfs_source_dir;
  String template_elf_path;
};

class NroBuilder {
private:
  String output_path;

  Error _convert_icon_to_jpeg(const String &p_src_icon_path, const String &p_out_jpg_path);
	Error _prepare_staging_romfs(const String &p_src_romfs_dir, const String &p_staging_romfs_dir);
	Error _generate_nacp(const String &p_nacptool_bin, const NroBuildConfig &p_config, const String &p_out_nacp_path);
	Error _generate_nro(const String &p_elf2nro_bin, const NroBuildConfig &p_config, const String &p_nacp_path, const String &p_romfs_dir, const String &p_icon_jpg_path);

public:
  void set_output_path(const String &p_path) { output_path = p_path; }
  String get_output_path() const { return output_path; };

	Error build(const NroBuildConfig &p_config);

  NroBuilder();
  ~NroBuilder();
};
