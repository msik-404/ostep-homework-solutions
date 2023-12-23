//
// Created by mateusz on 12/20/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    // char* args[] = {"ls", NULL};
    const int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    if (rc == 0)
    {
        const pid_t pid = wait(NULL);
        printf("child (pid:%d). Returned (pid:%d)\n", getpid(), pid);
        // execl("/bin/ls", "ls", NULL);
        // execv("/bin/ls", args);
        execlp("ls", NULL);
    }
    else
    {
        int wstatus;
        const pid_t pid = waitpid(rc, &wstatus, 0);
        printf("parent (pid:%d) of child (pid:%d). Returned (pid:%d). Status: (%d)\n", getpid(), rc, pid, wstatus);
    }

    return 0;
}
