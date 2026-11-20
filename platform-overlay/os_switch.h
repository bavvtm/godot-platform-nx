#pragma once

#include "switch_wrap.h"
#include "joypad_switch.h"

#include "core/os/os.h"
#include "drivers/unix/os_unix.h"

#include "audio_driver_switch.h"

class OS_Switch : public OS_Unix {
	MainLoop *main_loop = nullptr;

	JoypadSwitch *joypad_switch = nullptr;

	AudioDriverSwitch audio_driver;

	String s_executable_path = "sdmc:/switch/godot/godot_game.nro";

protected:
	virtual void initialize() override;
	virtual void finalize() override;

	virtual void initialize_joypads() override;

	virtual void delete_main_loop() override;
	virtual void set_main_loop(MainLoop *p_main_loop) override;

	virtual bool _check_internal_feature_support(const String &p_feature) override { return false; }

public:
	virtual String get_identifier() const override;
	virtual String get_name() const override;
	virtual String get_model_name() const override;
	virtual String get_distribution_name() const override;
	virtual String get_version() const override;
	virtual String get_unique_id() const override;
	virtual String get_processor_name() const override;
	virtual int get_processor_count() const override { return 3; }

	virtual void set_executable_path(const char *p_execpath);
	virtual String get_executable_path() const override;
	virtual String get_config_path() const override;
	virtual String get_data_path() const override;
	virtual String get_cache_path() const override;

	using OS_Unix::get_user_data_dir;
	virtual String get_user_data_dir() const override;

	virtual MainLoop *get_main_loop() const override;
	virtual Error get_entropy(uint8_t *r_buffer, int p_bytes) override;

	virtual void alert(const String &p_alert, const String &p_title = "ALERT!") override;

	String get_app_data_dir() const;

	void run();

	OS_Switch();
	virtual ~OS_Switch() override;
};
