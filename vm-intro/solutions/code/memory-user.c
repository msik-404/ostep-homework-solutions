//
// Created by mateusz on 12/29/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define MUTLI 1'000'000

long getTimeDiff(const struct timeval* start, const struct timeval* end)
{
    return (end->tv_sec - start->tv_sec) * 1000000 + end->tv_usec - start->tv_usec;
}

int main(int argc, char* argv[])
{
    pid_t pid = getpid();
    printf("%d\n", pid);

    if (argc != 3)
    {
        fprintf(stderr, "Two arguments is required.\n");
        exit(1);
    }

    const int memSize = atoi(argv[1]);
    const int time = atoi(argv[2]);
    if (memSize == 0 || time == 0)
    {
        fprintf(stderr, "Wrong data provided.\n");
        exit(1);
    }

    const int megaMemSize = memSize * MUTLI;

    char* array = malloc(megaMemSize * sizeof(char));
    if (array == NULL)
    {
        fprintf(stderr, "malloc failed.\n");
        exit(1);
    }

    struct timeval startTime;
    struct timeval endTime;
    gettimeofday(&startTime, NULL);
    endTime.tv_sec = startTime.tv_sec + time;
    endTime.tv_usec = startTime.tv_usec;

    while (getTimeDiff(&startTime, &endTime) > 0)
    {
        for (int i = 0; i < megaMemSize; ++i)
        {
            array[i] = (char) i;
        }
        gettimeofday(&startTime, NULL);
    }

    free(array);

    return 0;
}
