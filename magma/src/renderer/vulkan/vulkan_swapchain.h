#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

/**
 * @file vulkan_swapchain.h
 * @brief Funções para criar/destruir uma swapchain.
 */

#include "vulkan_types.inl"

void create_vulkan_swapchain(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain);
void recreate_vulkan_swapchain(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain);
void destroy_vulkan_swapchain(vulkan_context* context, vulkan_swapchain* swapchain);
b8 acquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout_ns, VkSemaphore image_available, VkFence fence, u32* image_index);
void swapchain_present(vulkan_context* context, vulkan_swapchain* swapchain, VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete, u32 present_image_index);

#endif // VULKAN_SWAPCHAIN_H
