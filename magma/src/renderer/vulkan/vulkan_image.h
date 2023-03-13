#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

/**
 * @file vulkan_image.h
 * @brief Funções para criar/destruir uma imagem.
 */

#include "vulkan_types.inl"

void create_vulkan_image(vulkan_context* context, VkImageType image_type, u32 width, u32 height, VkFormat format,
                         VkImageTiling image_tiling, VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_flags,
                         b8 create_view, VkImageAspectFlags image_aspect_flags, vulkan_image* image);

void create_vulkan_image_view(vulkan_context* context, VkFormat format, vulkan_image* image, VkImageAspectFlags image_aspect_flags);

void destroy_vulkan_image(vulkan_context* context, vulkan_image* image);

#endif // VULKAN_IMAGE_H
