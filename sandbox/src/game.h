#ifndef GAME_H
#define GAME_H

/**
 * @file game.h
 * @brief A estrutura do jogo é definida aqui. As funções declaradas devem possuir
 * os mesmos parâmetros que as funções declaradas em game_types. O funcionamento 
 * também é definido pelo usuário.
 */

#include <defines.h>
#include <game_types.h>

typedef struct game_state
{
    f32 delta_time;
} game_state;

b8 initialize(game* game_instance);
b8 update(game* game_instance, f32 delta_time);
b8 render(game* game_instance, f32 delta_time);
void on_resize(game* game_instance, u32 width, u32 height);

#endif // GAME_H
