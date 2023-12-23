//
// Created by mateusz on 12/23/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define ITER_AMOUNT 10'000'000

long getTimeDiff(const struct timeval* start, const struct timeval* end)
{
    return (end->tv_sec - start->tv_sec) * 1000000 + end->tv_usec - start->tv_usec;
}

int main(void)
{
    close(STDIN_FILENO);

    constexpr char message[2] = {'a', '\0'};
    char buf[2];

    int firsPipefd[2];
    int secondPipefd[2];

    const int rc = fork();

    if (rc < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    if (rc == 0)
    {
        printf("child (pid:%d)\n", getpid());

        close(firsPipefd[1]);
        close(secondPipefd[0]);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        for (int i = 0; i < ITER_AMOUNT; ++i)
        {
            read(firsPipefd[0], buf, 1);
            write(secondPipefd[1], message, 1);
        }
    }
    else
    {
        printf("parent (pid:%d) of child (pid:%d)\n", getpid(), rc);

        close(firsPipefd[0]);
        close(secondPipefd[1]);
        close(STDERR_FILENO);

        struct timeval firstTime;
        struct timeval secondTime;

        gettimeofday(&firstTime, NULL);

        for (int i = 0; i < ITER_AMOUNT; ++i)
        {
            write(firsPipefd[1], message, 1);
            read(secondPipefd[0], buf, 1);
        }

        gettimeofday(&secondTime, NULL);

        const long diff = getTimeDiff(&firstTime, &secondTime);

        const double avg = (double)diff / ITER_AMOUNT;

        printf("\nSTATS\ntime avg: %f microsec\ntime diff: %lu microsec\niter amount: %d\n\n", avg, diff, ITER_AMOUNT);
    }

    return 0;
}
