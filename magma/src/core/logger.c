#include "logger.h"
#include "../platform/platform.h"

// @TODO: temporario
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 initialize_logging()
{
    // @TODO
    return TRUE;
}

void logging_shutdown()
{
    // @TODO
}

void log_output(log_level level, const char* message, ...)
{
    const char* level_strings[6] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE" };
    b8 is_error = level < LOG_LEVEL_WARN;

    // Ninguém gosta de alocar na Heap :(
    const i32 message_size = 2000;
    char output_message[message_size];
    memset(output_message, 0, sizeof(output_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(output_message, message_size, message, arg_ptr);
    va_end(arg_ptr);

    char formatted_message[message_size];
    sprintf(formatted_message, "[%s]: %s\n", level_strings[level], output_message);

    if (is_error)
        platform_console_write_error(formatted_message, level);
    else
        platform_console_write(formatted_message, level);
}
