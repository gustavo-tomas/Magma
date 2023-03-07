#ifndef MGM_MEMORY_H
#define MGM_MEMORY_H

/**
 * @file mgm_memory.h
 * @brief Sistema de gerenciamento de memória. O objetivo é usar um sistema de tags
 * para identificar o tipo de alocação desejada (e seu respectivo free). Toda a 
 * memória alocada deve ser liberada no shutdown.
 */

#include "../defines.h"

typedef enum memory_tag
{
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS, // Esse é sempre o último elemento da lista
} memory_tag;

void initialize_memory();
void shutdown_memory();

/**
 * @brief As funções de gerenciamento chamam as funções de gerenciamento de memória
 * específicas de cada plataforma (ou seja, platform_allocate, etc), então são bem
 * similares a um wrapper. A diferença mais notável é o get_memory_usage, que 
 * retorna o uso de memória formatado.
 */

MGM_API void* mgm_allocate(u64 size, memory_tag tag);
MGM_API void  mgm_free(void* block, u64 size, memory_tag tag);
MGM_API void* mgm_zero_memory(void* block, u64 size);
MGM_API void* mgm_copy_memory(void* dest, i32 source, u64 size);
MGM_API void* mgm_set_memory(void* dest, i32 value, u64 size); // Função de debug

MGM_API char* get_memory_usage_str();

#endif // MGM_MEMORY_H
