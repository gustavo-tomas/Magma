#ifndef RENDERER_FRONTEND_H
#define RENDERER_FRONTEND_H

/**
 * @file renderer_frontend.h
 * @brief Interface do frontend do renderizador.
 */

#include "renderer_types.inl"

struct static_mesh_data;
struct platform_state;

b8 initialize_renderer(const char* application_name, struct platform_state* plat_state);
void shutdown_renderer();

b8 begin_frame_renderer(f32 delta_time);
b8 end_frame_renderer(f32 delta_time);
b8 draw_frame_renderer(render_packet* packet);

void on_resize_renderer(u16 width, u16 height);

#endif // RENDERER_FRONTEND_H
