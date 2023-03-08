#include "mgm_memory.h"

#include "logger.h"
#include "../platform/platform.h"

#include <stdio.h>
#include <string.h>

struct memory_stats
{
    u64 total_allocated;
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = 
{
    "UNKNOWN",
    "ARRAY",
    "VECTOR",
    "DICT",
    "RING_QUEUE",
    "BST",
    "STRING",
    "APPLICATION",
    "JOB",
    "TEXTURE",
    "MATERIAL_INSTANCE",
    "RENDERER",
    "GAME",
    "TRANSFORM",
    "ENTITY",
    "ENTITY_NODE",
    "SCENE"
};

static struct memory_stats stats;

void initialize_memory()
{
    platform_zero_memory(&stats, sizeof(stats));
}

void shutdown_memory()
{
    // @TODO: não faz nada por enquanto
}

MGM_API void* mgm_allocate(u64 size, memory_tag tag)
{
    if (tag == MEMORY_TAG_UNKNOWN)
        MGM_WARN("mgm_allocate chamada usando MEMORY_TAG_UNKNOWN!");

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    // @TODO: alinhamento de blocos
    void* block = platform_allocate(size, FALSE);
    platform_zero_memory(block, size);

    return block;
}

MGM_API void mgm_free(void* block, u64 size, memory_tag tag)
{
    if (tag == MEMORY_TAG_UNKNOWN)
        MGM_WARN("mgm_allocate chamada usando MEMORY_TAG_UNKNOWN!");

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    // @TODO: alinhamento de blocos
    platform_free(block, FALSE);
}

MGM_API void* mgm_zero_memory(void* block, u64 size)
{
    return platform_zero_memory(block, size);
}

MGM_API void* mgm_copy_memory(void* dest, const void* source, u64 size)
{
    return platform_copy_memory(dest, source, size);
}

MGM_API void* mgm_set_memory(void* dest, i32 value, u64 size)
{
    return platform_set_memory(dest, value, size);
}

MGM_API char* get_memory_usage_str()
{
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8000] = "Uso de memória (tags):\n";    
    u64 offset = strlen(buffer);

    for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; i++)
    {
        char unit[4] = "_iB";
        f32 amount = 1.0f;

        if (stats.tagged_allocations[i] >= gib)
        {
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (f32) gib;
        }

        else if (stats.tagged_allocations[i] >= mib)
        {
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (f32) mib;
        }

        else if (stats.tagged_allocations[i] >= kib)
        {
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (f32) kib;
        }

        else
        {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (f32) stats.tagged_allocations[i];
        }

        i32 length = snprintf(buffer + offset, 8000, " %s: %.3f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }

    char* output_string = buffer;
    return output_string;
}
