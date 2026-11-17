#pragma once

#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

class DeviceDiscoverySwitch : public Object {
	GDCLASS(DeviceDiscoverySwitch, Object);

private:
	PackedStringArray devices;
	bool devices_changed = false;
	bool quit_request = false;

	Ref<CoreBind::Mutex> device_lock;
	Ref<Thread> device_thread;

	void _poll_thread();

protected:
	static void _bind_methods();

public:
	void start();
	void stop();

	bool consume_devices_changed();
	int32_t get_device_count() const;
	String get_device_ip(int32_t p_index) const;

	Error deploy_pck(int32_t p_device_index, const String &p_pck_path);

	DeviceDiscoverySwitch();
	~DeviceDiscoverySwitch() override;
};
