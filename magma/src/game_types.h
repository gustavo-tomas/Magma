#ifndef GAME_TYPES_H
#define GAME_TYPES_H

/**
 * @file game_types.h
 * @brief Definição dos tipos e estruturas do jogo. As funções devem ser declaradas 
 * e definidas pelo usuário.
 */

#include "core/application.h"

/**
 * @brief Estrutura contendo as declarações das funções do jogo. O usuário deve
 * criar essas funções e definir seus funcionamentos.
 */
typedef struct game
{
    application_config app_config;
    b8 (*initialize) (struct game* game_instance);
    b8 (*update) (struct game* game_instance, f32 delta_time);
    b8 (*render) (struct game* game_instance, f32 delta_time);
    void (*on_resize) (struct game* game_instance, u32 width, u32 height);

    void* state;
} game;

#endif // GAME_TYPES_H
