#include "logger.h"

// @TODO: temporario
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 initialize_logging()
{
    // @TODO
    return TRUE;
}

void shutdown_logging()
{
    // @TODO
}

void log_output(log_level level, string message, ...)
{
    string level_strings[6] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE" };
    // b8 is_error = level < 2;

    // Ninguém gosta de alocar na Heap :(
    const int message_size = 2000;
    char output_message[message_size];
    memset(output_message, 0, sizeof(output_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(output_message, message_size, message, arg_ptr);
    va_end(arg_ptr);

    char formatted_message[message_size];
    sprintf(formatted_message, "[%s]: %s\n", level_strings[level], output_message);

    // @TODO: específico da plataforma
    switch (level)
    {
        case LOG_LEVEL_FATAL:   printf(COLOR_RED "%s" COLOR_RST, formatted_message); break;
        case LOG_LEVEL_ERROR:   printf(COLOR_RED "%s" COLOR_RST, formatted_message); break;
        case LOG_LEVEL_WARN:    printf(COLOR_YLW "%s" COLOR_RST, formatted_message); break;
        case LOG_LEVEL_INFO:    printf(COLOR_GRN "%s" COLOR_RST, formatted_message); break;
        case LOG_LEVEL_DEBUG:   printf(COLOR_BLU "%s" COLOR_RST, formatted_message); break;
        case LOG_LEVEL_TRACE:   printf(COLOR_WHT "%s" COLOR_RST, formatted_message); break;
        
        default: break;
    }
}
