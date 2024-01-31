//
// Created by mateusz on 1/30/24.
//

#ifndef VECTOR_H
#define VECTOR_H
#include <stddef.h>

typedef struct vector
{
    int* arr;
    size_t end;
    size_t size;
} vector_t;

void vec_init(vector_t* vector, size_t size);

void vec_free(vector_t* vector);

void append(vector_t* vector, int value);

int pop(vector_t* vector);

void set(const vector_t* vector, size_t idx, int value);

int get(const vector_t* vector, size_t idx);

#endif //VECTOR_H
