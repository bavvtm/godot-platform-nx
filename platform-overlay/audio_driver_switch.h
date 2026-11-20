#pragma once

#include "switch_wrap.h"

#include "core/os/mutex.h"
#include "core/os/thread.h"
#include "core/templates/vector.h"
#include "servers/audio/audio_server.h"

class AudioDriverSwitch : public AudioDriver {
	bool active = false;
	bool exit_thread = false;
	bool initialized = false;
	bool pause = false;

	Thread thread;
	Mutex mutex;

	enum {
		SAMPLE_RATE = 48000,
		BUFFER_FRAMES = 1024,
		BUFFER_COUNT = 3,
	};

	int16_t *buffers[BUFFER_COUNT] = {};
	int32_t *mixdown_buffer = nullptr;
	void *mempool_ptr = nullptr;
	size_t mempool_size = 0;
	int channels = 2;

#ifdef AUDREN_ENABLED
	LibnxAudioDriver drv;
	AudioDriverWaveBuf wavebufs[BUFFER_COUNT];
#else
	AudioOutBuffer audout_bufs[BUFFER_COUNT];
#endif

	Error _init_device();

#ifdef AUDREN_ENABLED
	static void _thread_func_audren(void *p_udata);
#else
	static void _thread_func_audout(void *p_udata);
#endif

public:
	virtual const char *get_name() const override {
#ifdef AUDREN_ENABLED
		return "Switch (audren)";
#else
		return "Switch (audout)";
#endif
	}

	virtual Error init() override;
	virtual void start() override { active = true; };
	virtual int get_mix_rate() const override { return SAMPLE_RATE; };
	virtual SpeakerMode get_speaker_mode() const override { return SPEAKER_MODE_STEREO; };

	virtual void lock() override { mutex.lock(); };
	virtual void unlock() override { mutex.unlock(); };
	virtual void finish() override;

	void set_pause(bool p_pause) { pause = p_pause; };
	void set_channel(int p_channel) { channels = p_channel; };

	AudioDriverSwitch();
	~AudioDriverSwitch();
};
