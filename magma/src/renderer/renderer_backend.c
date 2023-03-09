#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 create_renderer_backend(renderer_backend_type type, renderer_backend* backend)
{
    if (type == RENDERER_BACKEND_TYPE_VULKAN)
    {
        backend->initialize  = initialize_vulkan_renderer_backend;
        backend->shutdown    = shutdown_vulkan_renderer_backend;
        backend->begin_frame = begin_frame_vulkan_renderer_backend;
        backend->end_frame   = end_frame_vulkan_renderer_backend;
        backend->resize      = on_resize_vulkan_renderer_backend;

        return TRUE;
    }

    return FALSE;
}

void destroy_renderer_backend(renderer_backend* backend)
{
    backend->initialize = 0;
    backend->shutdown = 0;
    backend->resize = 0;

    backend->begin_frame = 0;
    backend->end_frame = 0;
}
