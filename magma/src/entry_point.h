#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

/**
 * @file entry_point.h
 * @brief É o ponto de entrada do jogo (contém a main). O funcionamento é definido
 * inteiramente pelo usuário, então, 'entry_point.c' deve ser definido na aplicação
 * do usuário.
 */

#include "game_types.h"

#include "core/application.h"
#include "core/systems_manager.h"
#include "core/logger.h"

/**
 * @brief Cria uma instância do jogo. Deve ser definido pelo usuário.
 *
 * @param out_game O jogo a ser criado.
 * @return b8 TRUE se a criação foi bem sucedida, FALSE caso contrário.
 */
extern b8 create_game(game* out_game);

int main()
{
    initialize_subsystems();

    game game_instance;

    if (!create_game(&game_instance))
    {
        MGM_FATAL("Erro ao criar o jogo!");
        return -1;
    }

    if (!game_instance.render || !game_instance.update || !game_instance.initialize || !game_instance.on_resize)
    {
        MGM_FATAL("Todos os atributos do jogo devem ser inicializados!");
        return -2;
    }

    // Inicialização
    if (!application_create(&game_instance))
    {
        MGM_ERROR("Erro ao criar a aplicação!");
        return 1;
    }

    // Game Loop
    if (!application_run())
    {
        MGM_INFO("Erro ao encerrar a aplicação!");
        return 2;
    }

    shutdown_subsystems();

    return 0;
}

#endif // ENTRY_POINT_H
