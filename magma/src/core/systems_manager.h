#ifndef SYSTEMS_MANAGER_H
#define SYSTEMS_MANAGER_H

/**
 * @file systems_manager.h
 * @brief Gerencia os subsistemas da engine. É responsável por reportar os erros
 * ocorridos durante o startup/shutdown.
 */

#include "../defines.h"

b8 initialize_subsystems();
b8 shutdown_subsystems();

#endif // SYSTEMS_MANAGER_H
