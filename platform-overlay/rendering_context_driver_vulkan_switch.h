#pragma once

#ifdef VULKAN_ENABLED

#include "drivers/vulkan/rendering_context_driver_vulkan.h"

class RenderingContextDriverVulkanSwitch : public RenderingContextDriverVulkan {
protected:
	virtual const char *_get_platform_surface_extension() const override;

public:
	struct WindowPlatformData {
		const void *window;
	};

	virtual SurfaceID surface_create(const void *p_platform_data) override;

	virtual void surface_destroy(SurfaceID p_surface) override;

	RenderingContextDriverVulkanSwitch();
	virtual ~RenderingContextDriverVulkanSwitch() override;
};

#endif // VULKAN_ENABLED
