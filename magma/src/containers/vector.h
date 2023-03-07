#ifndef VECTOR_H
#define VECTOR_H

/**
 * @file vector.h
 * @brief Vetor dinâmico. Cada vetor armazena informações sobre sua capacidade,
 * o número de elementos e o tamanho de cada elemento.
 */

#include "../defines.h"

enum
{
    VECTOR_CAPACITY,
    VECTOR_LENGTH,
    VECTOR_STRIDE,
    VECTOR_FIELD_LENGTH
};

MGM_API void* _vector_create(u64 length, u64 stride);
MGM_API void _vector_destroy(void* vector);

MGM_API u64 _vector_field_get(void* vector, u64 field);
MGM_API void _vector_field_set(void* vector, u64 field, u64 value);

MGM_API void* _vector_resize(void* vector);

MGM_API void* _vector_push(void* vector, const void* value_ptr);
MGM_API void _vector_pop(void* vector, void* dest);

MGM_API void* _vector_insert_at(void* vector, u64 index, void* value_ptr);
MGM_API void* _vector_pop_at(void* vector, u64 index, void* dest);

/**
 * @brief As macros implementam o funcionamento das funções 'privadas' (como _vector_create).
 * O objetivo é generalizar o tipo do vector para qualquer tipo desejado.
 */

#define VECTOR_DEFAULT_CAPACITY 1
#define VECTOR_RESIZE_FACTOR 2

#define vector_create(type)                     _vector_create(VECTOR_DEFAULT_CAPACITY, sizeof(type))
#define vector_reserve(type, capacity)          _vector_create(capacity, sizeof(type))
#define vector_destroy(vector)                  _vector_destroy(vector)
#define vector_pop(vector, value_ptr)           _vector_pop(vector, value_ptr)
#define vector_pop_at(vector, index, value_ptr) _vector_pop_at(vector, index, value_ptr)
#define vector_clear(vector)                    _vector_field_set(vector, VECTOR_LENGTH, 0)
#define vector_capacity(vector)                 _vector_field_get(vector, VECTOR_CAPACITY)
#define vector_length(vector)                   _vector_field_get(vector, VECTOR_LENGTH)
#define vector_stride(vector)                   _vector_field_get(vector, VECTOR_STRIDE)
#define vector_length_set(vector, value)        _vector_field_set(vector, VECTOR_LENGTH, value)

#define vector_push(vector, value)              \
    {                                           \
        typeof(value) temp = value;             \
        vector = _vector_push(vector, &temp);   \
    }

#define vector_insert_at(vector, index, value)              \
    {                                                       \
        typeof(value) temp = value;                         \
        vector = _vector_insert_at(vector, index, &temp);   \
    }

#endif // VECTOR_H
