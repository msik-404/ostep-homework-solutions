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
        data[i] = 0;
    }

    return 0;
}
