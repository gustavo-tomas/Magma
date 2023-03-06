#ifndef ASSERTS_H
#define ASSERTS_H

/**
 * @file asserts.h
 * @brief Definições de asserções e macros.
 */

#include "../defines.h"

// Comente a linha para desativar asserções
#define MGM_ASSERTIONS_ENABLED

#ifdef MGM_ASSERTIONS_ENABLED
#if _MSC_VER
    #include <intrin.h>
    #define debugBreak() __debugbreak()
#else
    #define debugBreak() __builtin_trap()
#endif

/**
 * @brief Envia uma mensagem
 *
 * @param expression Expressão avaliada em TRUE ou FALSE
 * @param message a mensagem da asserção
 * @param file arquivo em que a asserção ocorreu
 * @param line a linha em que a asserção ocorreu
 */
MGM_API void report_assertion_failure(string expression, string message, string file, i32 line);

/**
 * @brief Asserção de uma expressão booleana. Pode retornar uma mensagem.
 */
#define MGM_ASSERT(expr, message)                                           \
    {                                                                       \
        if (expr) { }                                                       \
        \
        else                                                                \
        {                                                                   \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);   \
            debugBreak();                                                   \
        }                                                                   \
    }

#ifdef _DEBUG
#define MGM_ASSERT_DEBUG(expr, message)                                     \
    {                                                                       \
        if (expr) { }                                                       \
        \
        else                                                                \
        {                                                                   \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);   \
            debugBreak();                                                   \
        }                                                                   \
    }
#else
#define MGM_ASSERT_DEBUG(expr, message)
#endif

#else
#define MGM_ASSERT(expr, message)
#define MGM_ASSERT_DEBUG(expr, message)
#endif
#endif // ASSERTS_H
