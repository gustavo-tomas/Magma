#ifndef VULKAN_RENDERPASS_H
#define VULKAN_RENDERPASS_H

/**
 * @file vulkan_renderpass.h
 * @brief Funções para criar/destruir passes de renderização.
 */

#include "vulkan_types.inl"

void create_vulkan_renderpass(vulkan_context* context, vulkan_renderpass* renderpass,
                              f32 x, f32 y, f32 w, f32 h, f32 r, f32 g, f32 b, f32 a,
                              f32 depth, u32 stencil);

void destroy_vulkan_renderpass(vulkan_context* context, vulkan_renderpass* renderpass);

void begin_vulkan_renderpass(vulkan_command_buffer* command_buffer, vulkan_renderpass* renderpass, VkFramebuffer frame_buffer);

void end_vulkan_renderpass(vulkan_command_buffer* command_buffer, vulkan_renderpass* renderpass);

#endif // VULKAN_RENDERPASS_H
