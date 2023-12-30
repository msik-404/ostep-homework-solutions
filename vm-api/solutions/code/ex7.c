//
// Created by mateusz on 12/29/23.
//

#include <stdlib.h>

#define SIZE 100

int main(void)
{
    int* data = malloc(SIZE * sizeof(int));
    for (int i = 0; i < SIZE; ++i)
    {
        data[i] = i;
    }

    int* ptr = data + 50;
    // int* ptr = (int64_t) data + 50 * sizeof(int);

    free(ptr);

    return 0;
}
