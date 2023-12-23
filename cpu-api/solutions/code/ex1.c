//
// Created by mateusz on 12/20/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{

    int x = 100;
    const int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } if (rc == 0) {
        printf("child (pid:%d). Old x is %d\n", getpid(), x);
        ++x;
        printf("child (pid:%d). New x is %d\n", getpid(), x);
    } else {
        printf("parent of %d (pid:%d). Old x is %d\n", rc, getpid(), x);
        --x;
        printf("parent (pid:%d). New x is %d\n", getpid(), x);
    }

    return 0;
}
