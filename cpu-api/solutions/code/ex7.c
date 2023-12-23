//
// Created by mateusz on 12/20/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    const int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    if (rc == 0)
    {
        const int status = close(STDOUT_FILENO);
        if (status == -1)
        {
            fprintf(stderr, "close failed\n");
            exit(1);
        }
        printf("child (pid:%d)\n", getpid());
    }
    else
    {
        printf("parent (pid:%d) of child (pid:%d)\n", getpid(), rc);
    }

    return 0;
}
