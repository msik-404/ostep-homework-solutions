//
// Created by mateusz on 12/29/23.
//

#include <stdlib.h>
#include <string.h>

int main(void)
{
    char* text = malloc(6 * sizeof(char));
    strcpy(text, "Siema");

    return 0;
}
