#ifndef VULKAN_TYPES_INL
#define VULKAN_TYPES_INL

/**
 * @file vulkan_types.inl
 * @brief Interface da estrutura necessária para criar um backend em vulkan.
 */

#include "../../defines.h"

#include <vulkan/vulkan.h>

typedef struct vulkan_context
{
    VkInstance instance;
    VkAllocationCallbacks* allocator;
} vulkan_context;

#endif // VULKAN_TYPES_INL
