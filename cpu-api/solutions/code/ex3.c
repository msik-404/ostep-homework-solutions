//
// Created by mateusz on 12/20/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{

    const int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } if (rc == 0) {
        printf("hello\n");
    } else {
        sleep(1);
        printf("goodbyte\n");
    }

    return 0;
}
