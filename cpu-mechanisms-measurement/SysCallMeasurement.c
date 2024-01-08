//
// Created by mateusz on 12/23/23.
//

#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define ITER_AMOUNT 10'000'000
#define FILENAME "sys_call_test.txt"

long getTimeDiff(const struct timespec* start, const struct timespec* end)
{
    return (end->tv_sec - start->tv_sec) * 1e9 + end->tv_nsec - start->tv_nsec;
}

int main(void)
{
    struct timespec firstTime, secondTime;

    const int fd = open(FILENAME, O_RDONLY | O_CREAT);

    clock_gettime(CLOCK_MONOTONIC, &firstTime);

    for (int i = 0; i < ITER_AMOUNT; ++i)
    {
        read(fd, NULL, 0);
    }

    clock_gettime(CLOCK_MONOTONIC, &secondTime);

    const long diff = getTimeDiff(&firstTime, &secondTime);

    const double avg = (double)diff / ITER_AMOUNT;

    printf("time avg: %f nanosec\ntime diff: %lu nanosec\niter amount: %d\n", avg, diff, ITER_AMOUNT);

    remove(FILENAME);

    return 0;
}
