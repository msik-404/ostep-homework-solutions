//
// Created by mateusz on 1/20/24.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _counter
{
    pthread_mutex_t mutex;
    int value;
} Counter;

void initCounter(Counter* counter)
{
    pthread_mutex_init(&counter->mutex, NULL);
    counter->value = 0;
}

void increment(Counter* counter)
{
    pthread_mutex_lock(&counter->mutex);
    counter->value++;
    pthread_mutex_unlock(&counter->mutex);
}

typedef struct _args
{
    Counter* counter;
    int iterations;
} Args;

void initArgs(Args* args, Counter* counter, const int iterations)
{
    args->counter = counter;
    args->iterations = iterations;
}

void* work(const void* args)
{
    const Args* arg = args;

    for (int i = 0; i < arg->iterations; ++i)
    {
        increment(arg->counter);
    }

    return NULL;
}

void getTimeDiff(const struct timespec* start, const struct timespec* end, struct timespec* result)
{
    result->tv_sec = end->tv_sec - start->tv_sec;
    result->tv_nsec = end->tv_nsec - start->tv_nsec;
    if (result->tv_nsec < 0)
    {
        --result->tv_sec;
        result->tv_nsec += 1000000000L;
    }
}

int main(const int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Wrong input format provided.\n");
        exit(1);
    }
    const int threadAmount = atoi(argv[1]);
    const int totalWork = atoi(argv[2]);
    if (threadAmount == 0 || totalWork == 0)
    {
        fprintf(stderr, "Wrong input format provided.\n");
        exit(1);
    }

    pthread_t threads[threadAmount];
    Counter counter;
    initCounter(&counter);

    const int workPerThread = totalWork / threadAmount;

    Args args;
    initArgs(&args, &counter, workPerThread);

    struct timespec firstTime, secondTime, diff;

    clock_gettime(CLOCK_MONOTONIC, &firstTime);

    for (int i = 0; i < threadAmount; ++i)
    {
        pthread_create(&threads[i], NULL, work, &args);
    }

    for (int i = 0; i < threadAmount; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &secondTime);

    getTimeDiff(&firstTime, &secondTime, &diff);

    printf("sec: %lu nsec: %lu  threads: %d work per thread %d total work: %d\n", diff.tv_sec, diff.tv_nsec,
           threadAmount, workPerThread, totalWork);

    return 0;
}
