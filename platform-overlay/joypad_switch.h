#pragma once

#include "core/error/error_list.h"
#include "core/error/error_macros.h"
#include "core/input/input.h"
#include "core/templates/hash_map.h"
#include "switch_wrap.h"

#define JOYPADS_MAX 8

class JoypadSwitch {
private:
	static inline HashMap<HidNpadButton, JoyButton> button_map;

	bool connected[JOYPADS_MAX] = {};
	u64 prev_buttons[JOYPADS_MAX] = {};
	PadState pads[JOYPADS_MAX];

	Input *input = nullptr;

	void _init_pad();

public:
	Error initialize();
	void process_pad();
	void reset_pad_state();

	JoypadSwitch();
	~JoypadSwitch();
};
