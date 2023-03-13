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

    VkFormat depth_format;
} vulkan_device;

typedef struct vulkan_image
{
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;
} vulkan_image;

typedef enum vulkan_renderpass_state
{
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} vulkan_renderpass_state;

typedef struct vulkan_renderpass
{
    VkRenderPass handle;
    f32 x, y, w, h;
    f32 r, g, b, a;
    f32 depth, stencil;
    vulkan_renderpass_state state;
} vulkan_renderpass;

typedef struct vulkan_swapchain
{
    VkSurfaceFormatKHR image_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 image_count;
    VkImage* images;
    VkImageView* views;
    vulkan_image depth_attachment;
} vulkan_swapchain;

typedef enum vulkan_command_buffer_state
{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
} vulkan_command_buffer_state;

typedef struct vulkan_command_buffer
{
    VkCommandBuffer handle;
    vulkan_command_buffer_state state;
} vulkan_command_buffer;

typedef struct vulkan_context
{
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    vulkan_device device;
    vulkan_swapchain swapchain;
    vulkan_renderpass main_renderpass;

    u32 image_index;
    u32 current_frame;

    b8 is_recreating_swapchain;

    u32 framebuffer_width;
    u32 framebuffer_height;

    i32 (*find_memory_index) (u32 type_filter, u32 property_flags);

    #if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
    #endif
} vulkan_context;

#endif // VULKAN_TYPES_INL
