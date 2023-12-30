//
// Created by mateusz on 12/29/23.
//

#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

int main(void)
{
    int* data = malloc(SIZE * sizeof(int));
    for (int i = 0; i < SIZE; ++i)
    {
        data[i] = 0;
    }

    free(data);

    printf("%d\n", data[5]);

    return 0;
}
