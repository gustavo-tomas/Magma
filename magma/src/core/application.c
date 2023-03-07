#include "application.h"
#include "logger.h"
#include "mgm_memory.h"
#include "event.h"
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
static b8 initialized = FALSE;
static application_state app_state;

MGM_API b8 application_create(game* game_instance)
{
    if (initialized)
    {
        MGM_ERROR("Erro ao criar aplicação (aplicação já foi inicializada)!");
        return FALSE;
    }

    app_state.game_instance = game_instance;

    // Inicialização dos subsistemas (StartUp)
    logging_initialize();
    input_initialize();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if (!event_initialize())
    {
        MGM_FATAL("Erro ao inicializar o sistema de eventos!");
        return FALSE;
    }

    if (!platform_startup(&app_state.platform, game_instance->app_config.name, 
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

    initialized = TRUE;

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

    app_state.is_running = FALSE;

    // Desligamento dos subsistemas (ShutDown)
    logging_shutdown();
    event_shutdown();
    input_shutdown();
    platform_shutdown(&app_state.platform);

    return TRUE;
}
