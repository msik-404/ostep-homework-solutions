//
// Created by mateusz on 1/8/24.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

unsigned long long getTimeDiff(const struct timespec* start, const struct timespec* end)
{
    return (end->tv_sec - start->tv_sec) * 1e9 + end->tv_nsec - start->tv_nsec;
}

int main(int argc, char* argv[])
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(7, &set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1)
    {
        perror(NULL);
        exit(1);
    }

    if (argc != 3)
    {
        fprintf(stderr, "Two int arguments are required.\n");
        exit(1);
    }

    const int NUMPAGES = atoi(argv[1]);
    const int NUMTRAILS = atoi(argv[2]);
    if (NUMPAGES == 0 || NUMTRAILS == 0)
    {
        fprintf(stderr, "Wrong input format provided.\n");
        exit(1);
    }

    const long PAGESIZE = sysconf(_SC_PAGESIZE);
    if (PAGESIZE == -1)
    {
        perror(NULL);
        exit(1);
    }
    const long JUMP = PAGESIZE / sizeof(int);

    int* a = calloc(NUMPAGES, PAGESIZE);
    if (a == NULL)
    {
        perror(NULL);
        exit(1);
    }

    struct timespec firstTime, secondTime;
    if (clock_gettime(CLOCK_MONOTONIC, &firstTime) == -1)
    {
        perror(NULL);
        exit(1);
    }

    for (int j = 0; j < NUMTRAILS; ++j)
    {
        for (int i = 0; i < JUMP * NUMPAGES; i += JUMP)
        {
            a[i] = 1;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &secondTime) == -1)
    {
        perror(NULL);
        exit(1);
    }

    const unsigned long long diff = getTimeDiff(&firstTime, &secondTime) / (NUMPAGES * NUMTRAILS);
    printf("cpu: %d pages: %d trails: %d nanosec: %llu\n", sched_getcpu(), NUMPAGES, NUMTRAILS, diff);

    free(a);

    return 0;
}
