#include "systems_manager.h"
#include "mgm_memory.h"
#include "logger.h"
#include "input.h"
#include "event.h"

static b8 is_initialized = FALSE;

// @TODO: tratar sucesso/falha
b8 initialize_subsystems()
{
    if (is_initialized)
        return FALSE;

    initialize_memory();
    MGM_INFO("Subsistema de Memória inicializado com sucesso!");

    initialize_logger();
    MGM_INFO("Subsistema de Log inicializado com sucesso!");

    initialize_input();
    MGM_INFO("Subsistema de IO inicializado com sucesso!");

    initialize_event();
    MGM_INFO("Subsistema de Eventos inicializado com sucesso!");

    MGM_INFO("Subsistemas inicializados com sucesso!");

    is_initialized = TRUE;
    return TRUE;
}

b8 shutdown_subsystems()
{
    if (!is_initialized)
        return FALSE;

    shutdown_event();
    MGM_INFO("Subsistema de Eventos desligado com sucesso!");
    
    shutdown_input();
    MGM_INFO("Subsistema de IO desligado com sucesso!");
    
    shutdown_logger();
    MGM_INFO("Subsistema de Log desligado com sucesso!");
    
    shutdown_memory();
    MGM_INFO("Subsistema de Memória desligado com sucesso!");

    MGM_INFO("Subsistemas desligados com sucesso!");

    return TRUE;
}
