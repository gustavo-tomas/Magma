#ifndef VULKAN_BACKEND_H
#define VULKAN_BACKEND_H

/**
 * @file vulkan_backend.h
 * @brief Assinatura das funções definidas em 'renderer_types.inl'.
 */

#include "../renderer_backend.h"

b8 initialize_vulkan_renderer_backend(struct renderer_backend* backend, const char* application_name, struct platform_state* plat_state);
void shutdown_vulkan_renderer_backend(struct renderer_backend* backend);

b8 begin_frame_vulkan_renderer_backend(struct renderer_backend* backend, f32 delta_time);
b8 end_frame_vulkan_renderer_backend(struct renderer_backend* backend, f32 delta_time);

void on_resize_vulkan_renderer_backend(struct renderer_backend* backend, u16 width, u16 height);

#endif // VULKAN_BACKEND_H
