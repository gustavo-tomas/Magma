#ifndef RENDERER_BACKEND_H
#define RENDERER_BACKEND_H

/**
 * @file renderer_backend.h
 * @brief Interface do backend do renderizador.
 */

#include "renderer_types.inl"

struct platform_state;

b8 create_renderer_backend(renderer_backend_type type, renderer_backend* backend);
void destroy_renderer_backend(renderer_backend* backend);

#endif // RENDERER_BACKEND_H
