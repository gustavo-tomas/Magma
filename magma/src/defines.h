#ifndef DEFINES_H
#define DEFINES_H

/**
 * @file defines.h
 * @brief Definição de diversos tipos e macros.
 */

#include <stdint.h>

// Inteiros sem sinal
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Inteiros com sinal
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Ponto flutuante
typedef float  f32;
typedef double f64;

// Booleanos
typedef int8_t  b8;
typedef int32_t b32;

// Cadeia de caracteres
typedef const char* string;

// Asserção de tipos
#if defined(__clang__) || defined(__gcc__)
    #define STATIC_ASSERT _Static_assert
#else
    #define STATIC_ASSERT static_assert
#endif

// Asserção de tamanhos
STATIC_ASSERT(sizeof(u8)  == 1, "Tamanho do u8 deve ser 1 byte!");
STATIC_ASSERT(sizeof(u16) == 2, "Tamanho do u16 deve ser 2 bytes!");
STATIC_ASSERT(sizeof(u32) == 4, "Tamanho do u32 deve ser 4 bytes!");
STATIC_ASSERT(sizeof(u64) == 8, "Tamanho do u64 deve ser 8 bytes!");

STATIC_ASSERT(sizeof(i8)  == 1, "Tamanho do i8 deve ser 1 byte!");
STATIC_ASSERT(sizeof(i16) == 2, "Tamanho do i16 deve ser 2 bytes!");
STATIC_ASSERT(sizeof(i32) == 4, "Tamanho do i32 deve ser 4 bytes!");
STATIC_ASSERT(sizeof(i64) == 8, "Tamanho do i64 deve ser 8 bytes!");

STATIC_ASSERT(sizeof(f32) == 4, "Tamanho do f32 deve ser 4 bytes!");
STATIC_ASSERT(sizeof(f64) == 8, "Tamanho do f64 deve ser 8 bytes!");

#define TRUE  1
#define FALSE 0

// Detecção de plataforma

// Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define MGM_PLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "(Windows) Sistema detectado não é 64-bits!"
    #endif

    // Linux
#elif defined(__linux__) || defined(__gnu_linux__)
    #define MGM_PLATFORM_LINUX 1
    #if defined(__ANDROID__)
        #define MGM_PLATFORM_ANDROID 1
    #endif

    // Unix
#elif defined(__unix__)
    #define MGM_PLATFORM_UNIX 1

    // Posix
#elif defined(_POSIX_VERSION)
    #define MGM_PLATFORM_POSIX 1

    // Apple
#elif __APPLE__
    #define MGM_PLATFORM_APPLE 1
    #include <TargetConditionals.h>

    // Simulador de IOS
    #if TARGET_IPHONE_SIMULATOR
        #define MGM_PLATFORM_IOS 1
        #define MGM_PLATFORM_IOS_SIMULATOR 1

        // IOS
    #elif TARGET_OS_IPHONE
        #define MGM_PLATFORM_IOS 1

        // Mac
    #elif TARGET_OS_MAC
        /* Vazio */

        // Plataforma Apple desconhecida
    #else
        #error "Plataforma Apple desconhecida!"
    #endif

    // Plataforma desconhecida
#else
    #error "Plataforma desconhecida!"

#endif

// Configuração de importação/exportação
#ifdef MGM_EXPORT
    #ifdef _MSC_VER
        #define MGM_API __declspec(dllexport)
    #else
        #define MGM_API __attribute__((visibility("default")))
    #endif

#else
    #ifdef _MSC_VER
        #define MGM_API __declspec(dllimport)
    #else
        #define MGM_API
    #endif
#endif

#endif // DEFINES_H
