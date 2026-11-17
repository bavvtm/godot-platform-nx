#include "device_discovery_switch.h"
#include "constants.h"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/packet_peer_udp.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void DeviceDiscoverySwitch::_poll_thread() {
	Ref<PacketPeerUDP> peer;
	peer.instantiate();
	if (peer->bind(28771) != OK) {
		return;
	}

	peer->set_broadcast_enabled(true);
	peer->set_dest_address("255.255.255.255", 28280);

	PackedByteArray ping_pkt;
	ping_pkt.resize(6);
	memcpy(ping_pkt.ptrw(), "nxboot", 6);

	while (!quit_request) {
		bool different = false;
		peer->put_packet(ping_pkt);
		OS::get_singleton()->delay_usec(500000);

		PackedStringArray new_devices;
		while (peer->get_available_packet_count() > 0) {
			peer->get_packet();
			String ip = peer->get_packet_ip();
			if (!ip.is_empty() && !new_devices.has(ip)) {
				new_devices.push_back(ip);
			}
		}
		new_devices.sort();

		device_lock->lock();
		if (new_devices.size() != devices.size()) {
			different = true;
		} else {
			for (int i = 0; i < devices.size(); i++) {
				if (devices[i] != new_devices[i]) {
					different = true;
					break;
				}
			}
		}
		if (different) {
			devices = new_devices;
			devices_changed = true;
		}
		device_lock->unlock();

		uint64_t wait_usec = 3000000;
		uint64_t start_usec = Time::get_singleton()->get_ticks_usec();
		while (Time::get_singleton()->get_ticks_usec() - start_usec < wait_usec) {
			OS::get_singleton()->delay_usec(100000);
			if (quit_request) {
				break;
			}
		}
	}
}

void DeviceDiscoverySwitch::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_poll_thread"), &DeviceDiscoverySwitch::_poll_thread);
}

void DeviceDiscoverySwitch::start() {
	if (device_thread.is_valid() && !device_thread->is_started()) {
		quit_request = false;
		device_thread->start(Callable(this, "_poll_thread"));
	}
}

void DeviceDiscoverySwitch::stop() {
	quit_request = true;
	if (device_thread.is_valid() && device_thread->is_started()) {
		device_thread->wait_to_finish();
	}
}

bool DeviceDiscoverySwitch::consume_devices_changed() {
	device_lock->lock();
	bool changed = devices_changed;
	if (changed) {
		devices_changed = false;
	}
	device_lock->unlock();
	return changed;
}

int32_t DeviceDiscoverySwitch::get_device_count() const {
	device_lock->lock();
	int32_t count = devices.size();
	device_lock->unlock();
	return count;
}

String DeviceDiscoverySwitch::get_device_ip(int32_t p_index) const {
	device_lock->lock();
	String ip = (p_index >= 0 && p_index < devices.size()) ? devices[p_index] : String();
	device_lock->unlock();
	return ip;
}

Error DeviceDiscoverySwitch::deploy_pck(int32_t p_device_index, const String &p_pck_path) {
	String nxlink = find_dkp_tool("nxlink");
	if (nxlink.is_empty()) {
		UtilityFunctions::push_error("'nxlink' binary not found.");
		return ERR_UNCONFIGURED;
	}

	String target_ip = get_device_ip(p_device_index);
	if (target_ip.is_empty()) {
		return ERR_INVALID_PARAMETER;
	}

	PackedStringArray args;
	args.push_back(p_pck_path);
	args.push_back("-a");
	args.push_back(target_ip);
	args.push_back("-p");
	args.push_back("TempGodot.pck");

	Array output;
	int exit_code = OS::get_singleton()->execute(nxlink, args, output, true);
	if (exit_code != 0) {
		UtilityFunctions::push_error("'nxlink' failed with exit code ", exit_code, ": ", output);
		return FAILED;
	}

	UtilityFunctions::print("deployed TempGodot to ", target_ip);
	return OK;
}

DeviceDiscoverySwitch::DeviceDiscoverySwitch() {
	device_lock.instantiate();
	device_thread.instantiate();
}

DeviceDiscoverySwitch::~DeviceDiscoverySwitch() {
	stop();
}
