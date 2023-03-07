#include "game.h"

#include <core/logger.h>

b8 initialize(game* game_instance)
{
    MGM_DEBUG("Jogo inicializado!");
    return TRUE;
}

b8 update(game* game_instance, f32 delta_time)
{
    return TRUE;
}

b8 render(game* game_instance, f32 delta_time)
{
    return TRUE;
}

void on_resize(game* game_instance, u32 width, u32 height)
{

}
