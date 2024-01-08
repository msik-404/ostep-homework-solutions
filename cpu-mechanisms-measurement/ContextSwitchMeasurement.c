//
// Created by mateusz on 12/23/23.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

#define ITER_AMOUNT 10'000'000

long getTimeDiff(const struct timespec* start, const struct timespec* end)
{
    return (end->tv_sec - start->tv_sec) * 1e9 + end->tv_nsec - start->tv_nsec;
}

int main(void)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(sched_getcpu(), &set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1)
    {
        fprintf(stderr, "sched_setaffinity failed.\n");
        exit(1);
    }

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

        struct timespec firstTime, secondTime;

        clock_gettime(CLOCK_MONOTONIC, &firstTime);

        for (int i = 0; i < ITER_AMOUNT; ++i)
        {
            write(firsPipefd[1], message, 1);
            read(secondPipefd[0], buf, 1);
        }

        clock_gettime(CLOCK_MONOTONIC, &secondTime);

        const long diff = getTimeDiff(&firstTime, &secondTime);

        const double avg = (double)diff / ITER_AMOUNT;

        printf("\nSTATS\ntime avg: %f nanosec\ntime diff: %lu nanosec\niter amount: %d\n\n", avg, diff, ITER_AMOUNT);
    }

    return 0;
}
