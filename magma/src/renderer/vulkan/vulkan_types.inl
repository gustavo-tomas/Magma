#ifndef VULKAN_TYPES_INL
#define VULKAN_TYPES_INL

/**
 * @file vulkan_types.inl
 * @brief Interface da estrutura necessária para criar um backend em vulkan.
 */

#include "../../defines.h"
#include "../../core/asserts.h"

#include <vulkan/vulkan.h>

/**
 * @brief Verifica se uma expressão que retorna VK_SUCCESS ou VK_ERROR foi bem sucedida.
 */
#define VK_CHECK(expression)                                                        \
{                                                                                   \
    MGM_ASSERT(expression == VK_SUCCESS, "Expressão não resultou em 'VK_SUCCESS'"); \
}

typedef struct vulkan_swapchain_support_info
{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    u32 present_mode_count;
    VkSurfaceFormatKHR* formats;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_device
{
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    vulkan_swapchain_support_info swapchain_support;

    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;

    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
} vulkan_device;

typedef struct vulkan_context
{
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    vulkan_device device;

    #if defined(_DEBUG)
        VkDebugUtilsMessengerEXT debug_messenger;
    #endif

} vulkan_context;

#endif // VULKAN_TYPES_INL
