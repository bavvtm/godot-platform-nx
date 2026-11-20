#include "rendering_context_driver_vulkan_switch.h"

#ifdef VULKAN_ENABLED

#include "switch_wrap.h"

#include <drivers/vulkan/godot_vulkan.h>

const char *RenderingContextDriverVulkanSwitch::_get_platform_surface_extension() const {
	return VK_NN_VI_SURFACE_EXTENSION_NAME; // "VK_NN_vi_surface"
}

RenderingContextDriver::SurfaceID RenderingContextDriverVulkanSwitch::surface_create(const void *p_platform_data) {
	ERR_FAIL_NULL_V(p_platform_data, SurfaceID());

	const WindowPlatformData *wpd = reinterpret_cast<const WindowPlatformData *>(p_platform_data);
	ERR_FAIL_NULL_V(wpd->window, SurfaceID());

	PFN_vkCreateViSurfaceNN func_vkCreateViSurfaceNN =
	    (PFN_vkCreateViSurfaceNN)vkGetInstanceProcAddr(instance_get(), "vkCreateViSurfaceNN");

	if (!func_vkCreateViSurfaceNN) {
		TRACE("Failed to get vkCreateViSurfaceNN function pointer.");
		return SurfaceID();
	}

	VkViSurfaceCreateInfoNN create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN;
	create_info.pNext = nullptr;
	create_info.flags = 0;
	create_info.window = const_cast<void *>(wpd->window);

	VkSurfaceKHR vk_surface = VK_NULL_HANDLE;
	VkResult err = func_vkCreateViSurfaceNN(
	    instance_get(),
	    &create_info,
	    get_allocation_callbacks(VK_OBJECT_TYPE_SURFACE_KHR),
	    &vk_surface);

	if (err != VK_SUCCESS) {
		TRACE("Failed to create Vulkan surface: %d", err);
		return SurfaceID();
	}

	Surface *surface = memnew(Surface);
	surface->vk_surface = vk_surface;

	return SurfaceID(surface);
}

void RenderingContextDriverVulkanSwitch::surface_destroy(SurfaceID p_surface) {
	if (p_surface == 0) {
		return;
	}

	Surface *surface = reinterpret_cast<Surface *>(p_surface);
	if (surface->vk_surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance_get(), surface->vk_surface, get_allocation_callbacks(VK_OBJECT_TYPE_SURFACE_KHR));
	}
	memdelete(surface);
}

RenderingContextDriverVulkanSwitch::RenderingContextDriverVulkanSwitch() {
	// Does nothing.
}

RenderingContextDriverVulkanSwitch::~RenderingContextDriverVulkanSwitch() {
	// Does nothing.
}

#endif // VULKAN_ENABLED
