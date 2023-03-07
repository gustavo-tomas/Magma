#include "vector.h"

#include "../core/mgm_memory.h"
#include "../core/logger.h"

void* _vector_create(u64 length, u64 stride)
{
    u64 header_size = VECTOR_FIELD_LENGTH * sizeof(u64);
    u64 vector_size = length * stride;
    u64* new_vector = mgm_allocate(header_size + vector_size, MEMORY_TAG_VECTOR);
   
    mgm_set_memory(new_vector, 0, header_size + vector_size);

    new_vector[VECTOR_CAPACITY] = length;
    new_vector[VECTOR_LENGTH] = 0;
    new_vector[VECTOR_STRIDE] = stride;

    return (void *) (new_vector + VECTOR_FIELD_LENGTH);
}

void _vector_destroy(void* vector)
{
    u64* header = (u64*) vector - VECTOR_FIELD_LENGTH;
    u64 header_size = VECTOR_FIELD_LENGTH * sizeof(u64);
    u64 total_size = header_size + header[VECTOR_CAPACITY] * header[VECTOR_STRIDE];
    
    mgm_free(header, total_size, MEMORY_TAG_VECTOR);
}

u64 _vector_field_get(void* vector, u64 field)
{
    u64* header = (u64*) vector - VECTOR_FIELD_LENGTH;
    return header[field];
}

void _vector_field_set(void* vector, u64 field, u64 value)
{
    u64* header = (u64*) vector - VECTOR_FIELD_LENGTH;
    header[field] = value;
}

void* _vector_resize(void* vector)
{
    u64 length = vector_length(vector);
    u64 stride = vector_stride(vector);
    void* temp = _vector_create((VECTOR_RESIZE_FACTOR * vector_capacity(vector)), stride);
    
    mgm_copy_memory(temp, vector, length * stride);

    _vector_field_set(temp, VECTOR_LENGTH, length);
    _vector_destroy(vector);

    return temp;
}

void* _vector_push(void* vector, const void* value_ptr)
{
    u64 length = vector_length(vector);
    u64 stride = vector_stride(vector);
    if (length >= vector_capacity(vector))
        vector = _vector_resize(vector);

    u64 addr = (u64) vector;
    addr += (length * stride);
    
    mgm_copy_memory((void*) addr, value_ptr, stride);

    _vector_field_set(vector, VECTOR_LENGTH, length + 1);

    return vector;
}

void _vector_pop(void* vector, void* dest)
{
    u64 length = vector_length(vector);
    u64 stride = vector_stride(vector);
    u64 addr = (u64) vector;

    addr += ((length - 1) * stride);

    mgm_copy_memory(dest, (void*) addr, stride);
    _vector_field_set(vector, VECTOR_LENGTH, length - 1);
}

void* _vector_insert_at(void* vector, u64 index, void* value_ptr)
{
    u64 length = vector_length(vector);
    u64 stride = vector_stride(vector);
    if (index >= length)
    {
        MGM_ERROR("Índice fora dos limites do vetor: Tamanho: %llu, Índice: %llu", length, index);
        return vector;
    }

    if (length >= vector_capacity(vector))
        vector = _vector_resize(vector);

    u64 addr = (u64) vector;

    if (index != length - 1)
    {
        mgm_copy_memory(
            (void*) (addr + ((index + 1) * stride)),
            (void*) (addr + (index * stride)),
            stride * (length - index));
    }

    mgm_copy_memory((void*) (addr + (index * stride)), value_ptr, stride);

    _vector_field_set(vector, VECTOR_LENGTH, length + 1);
    return vector;
}

void* _vector_pop_at(void* vector, u64 index, void* dest)
{
    u64 length = vector_length(vector);
    u64 stride = vector_stride(vector);
    if (index >= length)
    {
        MGM_ERROR("Índice fora dos limites do vetor: Tamanho: %llu, Índice: %llu", length, index);
        return vector;
    }

    u64 addr = (u64) vector;
    mgm_copy_memory(dest, (void*) (addr + (index * stride)), stride);

    if (index != length - 1)
    {
        mgm_copy_memory(
            (void*) (addr + (index * stride)),
            (void*) (addr + ((index + 1) * stride)),
            stride * (length - index));
    }

    _vector_field_set(vector, VECTOR_LENGTH, length - 1);
    return vector;
}
