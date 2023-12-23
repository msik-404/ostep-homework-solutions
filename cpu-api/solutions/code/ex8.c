//
// Created by mateusz on 12/20/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int pipefd[2];
    int status = pipe(pipefd);
    if (status == -1)
    {
        fprintf(stderr, "pipe creation failed\n");
        exit(1);
    }

    pid_t pids[2];

    for (int i = 0; i < 2; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            fprintf(stderr, "fork failed\n");
            abort();
        }
        if (pids[i] == 0)
        {
            if (i == 0) // read
            {
                dup2(pipefd[0], STDIN_FILENO);
                char buf[50];
                const ssize_t size = read(STDIN_FILENO, buf, 50);
                if (size == -1)
                {
                    fprintf(stderr, "read failed\n");
                    exit(1);
                }
                printf("This is the data read: \"%s\" with size: %lu by child (pid:%d).\n", buf, size, getpid());
            }
            if (i == 1) // write
            {
                dup2(pipefd[1], STDOUT_FILENO);
                printf("This is the data written by child (pid:%d).", getpid());
            }
            exit(0);
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        const pid_t pid = wait(&status);
        printf("Child with PID %d exited with status %d.\n", pid, status);
    }

    return 0;
}
