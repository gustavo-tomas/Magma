#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * @file platform.h
 * @brief Interface para abstrair o uso de códigos específicos para cada plataforma.
 */

#include "../defines.h"

typedef struct platform_state
{
    void* internal_state;
} platform_state;


/**
 * @brief Inicializa a janela e outros recursos, criando um estado com algumas referências relativas à plataforma.
 * 
 * @param plat_state O estado da plataforma
 * @param application_name O nome da aplicação
 * @param x Coordenada X da janela
 * @param y Coordenada Y da janela
 * @param width Largura da janela
 * @param height Altura da janela
 * @return b8 Status (Sucesso = 1, Erro = 0)
 */
b8 platform_startup(platform_state* plat_state, string application_name, i32 x, i32 y, i32 width, i32 height);

/**
 * @brief Desliga a janela e os recursos criados no startup
 * 
 * @param plat_state O estado da plataforma
 */
void platform_shutdown(platform_state* plat_state);

b8 platform_dispatch_messages(platform_state* plat_state);

// Alocação de memória

void* platform_allocate(u64 size, b8 aligned);
void platform_freeze(void* block, b8 aligned);
void* platform_zero_memory(void* block, u64 size);
void* platform_copy_memory(void* dest, i32 source, u64 size);
void* platform_set_memory(void* dest, i32 value, u64 size);

// Interação com o console

void platform_console_write(string message, u8 color);
void platform_console_write_error(string message, u8 color);

f64 platform_get_absolute_time();

void platform_sleep(u64 ms);

#endif // PLATFORM_H
