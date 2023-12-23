//
// Created by mateusz on 12/23/23.
//

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include<sys/time.h>

#define ITER_AMOUNT 10'000'000
#define FILENAME "sys_call_test.txt"

long getTimeDiff(const struct timeval* start, const struct timeval* end)
{
    return (end->tv_sec - start->tv_sec) * 1000000 + end->tv_usec - start->tv_usec;
}

int main(void)
{
    struct timeval firstTime;
    struct timeval secondTime;

    const int fd = open(FILENAME, O_RDONLY | O_CREAT);

    gettimeofday(&firstTime, NULL);
    for (int i = 0; i < ITER_AMOUNT; ++i)
    {
        read(fd, NULL, 0);
    }
    gettimeofday(&secondTime, NULL);

    const long diff = getTimeDiff(&firstTime, &secondTime);

    const double avg = (double)diff / ITER_AMOUNT;

    printf("time avg: %f microsec\ntime diff: %lu microsec\niter amount: %d\n", avg, diff, ITER_AMOUNT);

    remove(FILENAME);

    return 0;
}
