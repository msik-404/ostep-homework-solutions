//
// Created by mateusz on 12/20/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    const char* filename = "file.txt";
    remove(filename);
    const int fd = open(filename, O_CREAT | O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "file creation failed\n");
        exit(1);
    }

    const int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    if (rc == 0)
    {
        const ssize_t dataSize = snprintf(NULL, 0, "child (pid:%d)\n", getpid());
        char* buf = malloc(dataSize + 1);
        snprintf(buf, dataSize + 1, "child (pid:%d)\n", getpid());
        const ssize_t writeSize = write(fd, buf, sizeof(buf));
        if (writeSize < 0)
        {
            fprintf(stderr, "child write failed\n");
            exit(1);
        }
    }
    else
    {
        const ssize_t dataSize = snprintf(NULL, 0, "parent (pid:%d)\n", getpid());
        char* buf = malloc(dataSize + 1);
        snprintf(buf, dataSize + 1, "parent (pid:%d)\n", getpid());
        const ssize_t writeSize = write(fd, buf, sizeof(buf));
        if (writeSize < 0)
        {
            fprintf(stderr, "parent write failed\n");
            exit(1);
        }
    }

    return 0;
}
