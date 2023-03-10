#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

/**
 * @file vulkan_device.h
 * @brief Definições relevantes para criação de um dispositivo em vulkan.
 */

#include "vulkan_types.inl"

typedef struct vulkan_physical_device_requirements
{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    b8 sampler_anisotropy;
    b8 discrete_gpu;

    const char** device_extension_names;
} vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info
{
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

b8 create_vulkan_device(vulkan_context* context);

void destroy_vulkan_device(vulkan_context* context);

b8 select_physical_device(vulkan_context* context);

b8 physical_device_meets_requirements(VkPhysicalDevice device, VkSurfaceKHR surface, 
                                      const VkPhysicalDeviceProperties* properties, 
                                      const VkPhysicalDeviceFeatures* features,
                                      const vulkan_physical_device_requirements* requirements,
                                      vulkan_physical_device_queue_family_info* queue_info,
                                      vulkan_swapchain_support_info* swapchain_support);

void query_device_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, vulkan_swapchain_support_info* support_info);

#endif // VULKAN_DEVICE_H
