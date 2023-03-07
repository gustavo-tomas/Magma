#include "game.h"

#include <entry_point.h>

#include <core/mgm_memory.h>

// Ponto de entrada para criar o jogo
b8 create_game(game* out_game)
{
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;
    out_game->app_config.name = "Magma Sandbox";

    out_game->initialize = initialize;
    out_game->update = update;
    out_game->render = render;
    out_game->on_resize = on_resize;

    out_game->state = mgm_allocate(sizeof(game_state), MEMORY_TAG_GAME);

    return TRUE;
}
