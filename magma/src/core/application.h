#ifndef APPLICATION_H
#define APPLICATION_H

/**
 * @file application.h
 * @brief Camada de aplicação. O objetivo é abstrair a construção da aplicação do
 * resto da engine (a engine não precisa saber se a janela foi criada em linux
 * ou windows).
 */

#include "../defines.h"

struct game;

/**
 * @brief Configuração da aplicação. Armazena argumentos relativos à criação da
 * aplicação, como nome, posição, largura e altura.
 */
typedef struct application_config
{
    i16 start_pos_x;
    i16 start_pos_y;
    i16 start_width;
    i16 start_height;
    char* name;
} application_config;

MGM_API b8 application_create(struct game* game_instance);
MGM_API b8 application_run();

#endif // APPLICATION_H
