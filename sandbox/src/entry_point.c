#include "game.h"

#include <entry_point.h>

// Ponto de entrada para criar o jogo
b8 create_game(game* out_game)
{
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;
    out_game->app_config.name = "Magma Sandbox";

    out_game->initialize = initialize_game;
    out_game->update = update_game;
    out_game->render = render_game;
    out_game->on_resize = on_resize_game;

    out_game->state = mgm_allocate(sizeof(game_state), MEMORY_TAG_GAME);

    return TRUE;
}
