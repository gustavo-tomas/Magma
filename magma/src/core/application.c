#include "application.h"

#include "clock.h"
#include "input.h"
#include "logger.h"
#include "mgm_memory.h"

#include "../game_types.h"
#include "../platform/platform.h"

#include "../renderer/renderer_frontend.h"

typedef struct application_state
{
    game* game_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    clock clock;
    f64 last_time;
} application_state;

// Aplicação é inicializada SOMENTE 1 vez
static b8 is_initialized = FALSE;
static application_state app_state;

MGM_API b8 create_application(game* game_instance)
{
    if (is_initialized)
    {
        MGM_ERROR("Erro ao criar aplicação (aplicação já foi inicializada)!");
        return FALSE;
    }

    app_state.game_instance = game_instance;
    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    register_event(EVENT_CODE_APPLICATION_QUIT, 0, on_event_application);
    register_event(EVENT_CODE_KEY_PRESSED, 0, on_key_application);
    register_event(EVENT_CODE_KEY_RELEASED, 0, on_key_application);

    // @TODO: algum dia mover pra initialize_subsystems?
    if (!initialize_platform(&app_state.platform, app_state.game_instance->app_config.name, 
                          app_state.game_instance->app_config.start_pos_x, app_state.game_instance->app_config.start_pos_y,
                          app_state.game_instance->app_config.start_width, app_state.game_instance->app_config.start_height))
    {
        return FALSE;
    }

    // @TODO: também mover para initialize_subsystems?
    if (!initialize_renderer(app_state.game_instance->app_config.name, &app_state.platform))
    {
        MGM_FATAL("Erro ao inicializar o renderizador!");
        return FALSE;
    }

    if (!app_state.game_instance->initialize(app_state.game_instance))
    {
        MGM_FATAL("Erro ao inicializar o jogo!");
        return FALSE;
    }

    app_state.game_instance->on_resize(app_state.game_instance, app_state.width, app_state.height);

    is_initialized = TRUE;

    return TRUE;
}

MGM_API b8 run_application()
{
    MGM_INFO(get_memory_usage_str());

    start_clock(&app_state.clock);
    update_clock(&app_state.clock);
    app_state.last_time = app_state.clock.elapsed_time;

    f64 target_frame_time = 1.0f / 60;

    // Game Loop
    while (app_state.is_running)
    {
        if (!platform_dispatch_messages(&app_state.platform))
        {
            app_state.is_running = FALSE;
            break;
        }

        if (!app_state.is_suspended)
        {
            // Update ----------------------------------------------------------
            update_clock(&app_state.clock);
            f64 current_time = app_state.clock.elapsed_time;
            f64 delta_time = current_time - app_state.last_time;
            f64 frame_start_time = platform_get_absolute_time();

            if (!app_state.game_instance->update(app_state.game_instance, (f32) delta_time))
            {
                MGM_FATAL("Erro no Game Update!");
                app_state.is_running = FALSE;
                break;
            }

            // Render ----------------------------------------------------------
            if (!app_state.game_instance->render(app_state.game_instance, (f32) delta_time))
            {
                MGM_FATAL("Erro no Game Render!");
                app_state.is_running = FALSE;
                break;
            }

            // @TODO: pacote não deve ser criado desse jeito
            render_packet packet = { .delta_time = delta_time };
            draw_frame_renderer(&packet);

            // Calcula o tempo do frame
            f64 frame_end_time = platform_get_absolute_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            f64 remaining_time = target_frame_time - frame_elapsed_time;

            if (remaining_time > 0)
            {
                u64 remaining_ms = remaining_time * 1000;

                b8 limit_frames = FALSE;
                if (remaining_ms > 0 && limit_frames)
                    platform_sleep(remaining_ms - 1);
            }

            // Input é o último a ser atualizado
            update_input(delta_time);

            app_state.last_time = current_time;
        }
    }

    // @TODO não é necessário
    unregister_event(EVENT_CODE_APPLICATION_QUIT, 0, on_event_application);
    unregister_event(EVENT_CODE_KEY_PRESSED, 0, on_key_application);
    unregister_event(EVENT_CODE_KEY_RELEASED, 0, on_key_application);

    shutdown_renderer();

    // @TODO: algum dia mover pra initialize_subsystems
    shutdown_platform(&app_state.platform);

    return TRUE;
}

b8 on_event_application(u16 code, void* sender, void* listener_instance, event_context context)
{
    switch (code)
    {
        case EVENT_CODE_APPLICATION_QUIT:
            app_state.is_running = FALSE;
            return TRUE;
            break;
        
        default:
            break;
    }

    return FALSE;
}

b8 on_key_application(u16 code, void* sender, void* listener_instance, event_context context)
{
    switch (code)
    {
        // Tecla pressionada
        case EVENT_CODE_KEY_PRESSED:
        {
            u16 key_code = context.data.u16[0];
            switch (key_code)
            {
                // Evento de saída
                case KEY_ESCAPE:
                case KEY_Q:
                    fire_event(EVENT_CODE_APPLICATION_QUIT, 0, context);
                    return TRUE;
                    break;

                // Testes
                case KEY_SPACE:
                    MGM_DEBUG("> Tecla 'SPACE' pressionada!");
                    break;
                
                default:
                    MGM_DEBUG("Tecla '%c' pressionada!", key_code);
                    break;
            }
            break;
        } break;

        // Tecla levantada
        case EVENT_CODE_KEY_RELEASED:
        {
            u16 key_code = context.data.u16[0];
            switch (key_code)
            {
                case KEY_SPACE:
                    MGM_DEBUG("> Tecla 'SPACE' levantada!");
                    break;
                
                default:
                    MGM_DEBUG("Tecla '%c' levantada!", key_code);
                    break;
            }
            break;
        } break;
        
        default:
            break;
    }

    return FALSE;
}
