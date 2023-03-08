#include "application.h"
#include "logger.h"
#include "mgm_memory.h"
#include "input.h"

#include "../game_types.h"
#include "../platform/platform.h"

typedef struct application_state
{
    game* game_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_frame;
} application_state;

// Aplicação é inicializada SOMENTE 1 vez
static b8 is_initialized = FALSE;
static application_state app_state;

MGM_API b8 application_create(game* game_instance)
{
    if (is_initialized)
    {
        MGM_ERROR("Erro ao criar aplicação (aplicação já foi inicializada)!");
        return FALSE;
    }

    app_state.game_instance = game_instance;
    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    register_event(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    register_event(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    register_event(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    // @TODO: algum dia mover pra initialize_subsystems
    if (!initialize_platform(&app_state.platform, game_instance->app_config.name, 
                          game_instance->app_config.start_pos_x, game_instance->app_config.start_pos_y,
                          game_instance->app_config.start_width, game_instance->app_config.start_height))
    {
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

MGM_API b8 application_run()
{
    MGM_INFO(get_memory_usage_str());

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
            // Update
            if (!app_state.game_instance->update(app_state.game_instance, 0.f))
            {
                MGM_FATAL("Erro no Game Update!");
                app_state.is_running = FALSE;
                break;
            }

            // Render
            if (!app_state.game_instance->render(app_state.game_instance, 0.f))
            {
                MGM_FATAL("Erro no Game Render!");
                app_state.is_running = FALSE;
                break;
            }

            // Input é o último a ser atualizado
            input_update(0); // @TODO: delta time!
        }
    }

    // @TODO não é necessário
    unregister_event(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    unregister_event(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    unregister_event(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    // @TODO: algum dia mover pra initialize_subsystems
    shutdown_platform(&app_state.platform);

    return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_instance, event_context context)
{
    switch (code)
    {
        case EVENT_CODE_APPLICATION_QUIT:
            MGM_INFO("Evento de Saída - Terminando aplicação!");
            app_state.is_running = FALSE;
            return TRUE;
            break;
        
        default:
            break;
    }

    return FALSE;
}

b8 application_on_key(u16 code, void* sender, void* listener_instance, event_context context)
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
