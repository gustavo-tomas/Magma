#ifndef APPLICATION_H
#define APPLICATION_H

/**
 * @file application.h
 * @brief Camada de aplicação. O objetivo é abstrair a construção da aplicação do
 * resto da engine (a engine não precisa saber se a janela foi criada em linux
 * ou windows).
 */

#include "../defines.h"
#include "event.h"

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

MGM_API b8 create_application(struct game* game_instance);
MGM_API b8 run_application();

// Eventos
b8 on_event_application(u16 code, void* sender, void* listener_instance, event_context context);
b8 on_key_application(u16 code, void* sender, void* listener_instance, event_context context);

#endif // APPLICATION_H
