#ifndef LOGGER_H
#define LOGGER_H

/**
 * @file logger.h
 * @brief API para interação com o terminal.
 */

#include "../defines.h"

// FATAL e ERROR são inclusas em qualquer distribuição
#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// DEBUG e TRACE são desativadas para Release
#if MGM_RELEASE == 1
    #define LOG_DEBUG_ENABLED 0
    #define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} log_level;

b8 initialize_logger();
void shutdown_logger();

/**
 * @brief Imprime a mensagem passada com uma classificação
 *
 * @param level A classificação da mensagem
 * @param message A mensagem para imprimir
 * @param ... Argumentos opcionais
 */
MGM_API void log_output(log_level level, const char* message, ...);

// Erro fatal
#ifndef MGM_FATAL
    #define MGM_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);
#endif

// Erro padrão
#ifndef MGM_ERROR
    #define MGM_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

// Alerta
#if LOG_WARN_ENABLED == 1
    #define MGM_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
    #define MGM_WARN(message, ...)
#endif

// Info
#if LOG_INFO_ENABLED == 1
    #define MGM_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
    #define MGM_INFO(message, ...)
#endif

// Debug
#if LOG_DEBUG_ENABLED == 1
    #define MGM_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
    #define MGM_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
    #define MGM_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
    #define MGM_TRACE(message, ...)
#endif

#endif // LOGGER_H
