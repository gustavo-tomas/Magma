#include "game.h"

#include <entry_point.h>

// Ponto de entrada para criar o jogo
b8 create_game(game* game)
{
    game->app_config.start_pos_x = 100;
    game->app_config.start_pos_y = 100;
    game->app_config.start_width = 1280;
    game->app_config.start_height = 720;
    game->app_config.name = "Magma Sandbox";

    game->initialize = initialize_game;
    game->update = update_game;
    game->render = render_game;
    game->on_resize = on_resize_game;

    game->state = mgm_allocate(sizeof(game_state), MEMORY_TAG_GAME);

    return TRUE;
}
