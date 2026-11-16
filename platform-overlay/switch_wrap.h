#pragma once

#include <cstddef>
#define Mutex LibnxMutex
#define Semaphore LibnxSemaphore
#define Thread LibnxThread
#define AudioDriver LibnxAudioDriver

#include <switch.h>
#undef Thread
#undef Mutex
#undef Semaphore
#undef AudioDriver

#if defined(DEBUG_ENABLED) || defined(ENABLE_NXLINK)
#define TRACE(fmt, ...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)
#else
#define TRACE(fmt, ...) ((void)0)
#endif

namespace nx {

constexpr u32 MAX_TOUCH_POINTS = 16;

enum HardwareModel : u32 {
	HARDWARE_ICOSA  = 0, // V1 (Erista)
	HARDWARE_COPPER = 1, // Erista DevKit
	HARDWARE_HOAG   = 2, // Switch Lite
	HARDWARE_IOWA   = 3, // V2 (Mariko)
	HARDWARE_CALCIO = 4, // Mariko DevKit
	HARDWARE_AULA   = 5, // Switch OLED
};

bool firmware_version(char *r_buf, size_t p_len);
u32 hardware_type();

bool is_oled_model();
bool is_lite_model();

void ensure_dir(const char *p_path);
void init_file_logging_from_argv(const char *p_argv0);

inline uint64_t millis() {
	return armTicksToNs(armGetSystemTick()) / 1000000ULL;
}

} // namespace nx
