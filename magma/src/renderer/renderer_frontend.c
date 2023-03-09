#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "../core/logger.h"
#include "../core/mgm_memory.h"

static renderer_backend* backend = 0;

b8 initialize_renderer(const char* application_name, struct platform_state* plat_state)
{
    backend = mgm_allocate(sizeof(renderer_backend), MEMORY_TAG_RENDERER);
    backend->plat_state = plat_state;

    // @TODO: expandir para outras APIs!
    create_renderer_backend(RENDERER_BACKEND_TYPE_VULKAN, backend);
    backend->frame_number = 0;

    if (!backend->initialize(backend, application_name, plat_state))
    {
        MGM_FATAL("Erro ao criar o backend do renderizador!");
        return FALSE;
    }

    return TRUE;
}

void shutdown_renderer()
{
    backend->shutdown(backend);
    mgm_free(backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);
}

b8 begin_frame_renderer(f32 delta_time)
{
    return backend->begin_frame(backend, delta_time);
}

b8 end_frame_renderer(f32 delta_time)
{
    b8 result = backend->end_frame(backend, delta_time);
    backend->frame_number++;
    
    return result;
}

b8 draw_frame_renderer(render_packet* packet)
{
    if (begin_frame_renderer(packet->delta_time))
    {
        b8 result_ok = end_frame_renderer(packet->delta_time);

        if (!result_ok)
        {
            MGM_ERROR("Erro ao chamar 'end_frame_renderer'!");
            return FALSE;
        }
    }

    return TRUE;
}

void on_resize_renderer(u16 width, u16 height)
{

}
