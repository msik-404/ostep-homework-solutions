//
// Created by mateusz on 12/30/23.
//

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

#define GROWTH_FACTOR 1.5

void vec_init(vector_t* vector, const size_t size)
{
    if (size == 0)
    {
        fprintf(stderr, "Vector initialization failed.\n");
        exit(1);
    }
    vector->arr = malloc(size * sizeof(int));
    if (vector->arr == NULL)
    {
        fprintf(stderr, "Vector initialization failed.\n");
        exit(1);
    }
    vector->size = size;
    vector->end = 0;
}

void vec_free(vector_t* vector)
{
    free(vector->arr);
    vector->arr = NULL;
    vector->size = 0;
    vector->end = 0;
}

void append(vector_t* vector, const int value)
{
    if (vector->end == vector->size)
    {
        const size_t newSize = ceil(vector->size * GROWTH_FACTOR);
        vector->arr = realloc(vector->arr, newSize * sizeof(int));
        if (vector->arr == NULL)
        {
            fprintf(stderr, "Vector size increase failed.\n");
            exit(1);
        }
        vector->size = newSize;
    }
    else if (vector->end > vector->size)
    {
        fprintf(stderr, "end is grater than size.\n");
        exit(1);
    }
    vector->arr[vector->end++] = value;
}

int pop(vector_t* vector)
{
    if (vector->end == 0)
    {
        fprintf(stderr, "Vector is empty.\n");
        exit(1);
    }
    return vector->arr[--vector->end];
}

void set(const vector_t* vector, const size_t idx, const int value)
{
    if (idx > vector->end)
    {
        fprintf(stderr, "Idx is past the end of vector.");
        exit(1);
    }
    vector->arr[idx] = value;
}

int get(const vector_t* vector, const size_t idx)
{
    if (idx > vector->end)
    {
        fprintf(stderr, "Idx is past the end of vector.");
        exit(1);
    }
    return vector->arr[idx];
}

int main(void)
{
    vector_t vector;

    vec_init(&vector, 2);

    append(&vector, 10);
    append(&vector, 11);

    assert(2 == vector.end);

    assert(11 == pop(&vector));
    assert(10 == pop(&vector));

    assert(0 == vector.end);

    append(&vector, 12);
    append(&vector, 13);

    assert(2 == vector.end);

    assert(12 == get(&vector, 0));
    set(&vector, 1, 1);

    assert(1 == get(&vector, 1));

    assert(2 == vector.size);

    vec_free(&vector);

    vec_init(&vector, 1);
    assert(1 == vector.size);

    for (int i = 0; i < 10; ++i)
    {
        append(&vector, i);
        assert(i == get(&vector, i));
    }

    assert(12 == vector.size);

    vec_free(&vector);

    return 0;
}
