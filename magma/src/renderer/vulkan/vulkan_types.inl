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
 * @brief Verifica se uma expressão foi bem sucedida.
 */
#define VK_CHECK(expression)                                                        \
{                                                                                   \
    MGM_ASSERT(expression == VK_SUCCESS, "Expressão não resultou em 'VK_SUCCESS'"); \
}

typedef struct vulkan_context
{
    VkInstance instance;
    VkAllocationCallbacks* allocator;

    #if defined(_DEBUG)
        VkDebugUtilsMessengerEXT debug_messenger;
    #endif

} vulkan_context;

#if defined(_DEBUG)
    #include "../../core/logger.h"

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_types,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data)
        {
            switch (message_severity)
            {
                default:
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    MGM_ERROR(callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    MGM_WARN(callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    MGM_INFO(callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    MGM_TRACE(callback_data->pMessage);
                    break;
            }
            return VK_FALSE;
        }
#endif

#endif // VULKAN_TYPES_INL
