#include "asserts.h"
#include "logger.h"

void report_assertion_failure(string expression, string message, string file, i32 line)
{
    log_output(LOG_LEVEL_FATAL, "Falha na asserção: %s, mensagem: '%s', arquivo: %s, line %d\n", expression, message, file, line);
}
