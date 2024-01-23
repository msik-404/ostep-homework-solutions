//
// Created by mateusz on 1/20/24.
//

/*
 * sec: 5 nsec: 25193804  threads: 1 work per thread 10000000
 * sec: 5 nsec: 607198200  threads: 2 work per thread 10000000
 */

#define _GNU_SOURCE

#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define LOCAL_COUNTER_SIZE 1'000

typedef struct _counter
{
    pthread_mutex_t globalMutex;
    pthread_mutex_t* localMutexes;

    int globalValue;
    int* localValues;

    int threadAmount;
} Counter;

void initCounter(Counter* counter, const int threadAmount)
{
    counter->localMutexes = malloc(threadAmount * sizeof(pthread_mutex_t));
    counter->localValues = malloc(threadAmount * sizeof(int));
    counter->threadAmount = threadAmount;

    pthread_mutex_init(&counter->globalMutex, NULL);
    counter->globalValue = 0;
    for (int i = 0; i < threadAmount; ++i)
    {
        pthread_mutex_init(&counter->localMutexes[i], NULL);
        counter->localValues[i] = 0;
    }
}

void clearCounter(Counter* counter)
{
    pthread_mutex_destroy(&counter->globalMutex);
    for (int i = 0; i < counter->threadAmount; ++i)
    {
        pthread_mutex_destroy(&counter->localMutexes[i]);
    }
    free(counter->localMutexes);
    free(counter->localValues);
}

void flush(Counter* counter, const int id, const bool isWithinLocalLock)
{
    pthread_mutex_lock(&counter->globalMutex);
    counter->globalValue += counter->localValues[id];
    pthread_mutex_unlock(&counter->globalMutex);
    if (isWithinLocalLock)
    {
        counter->localValues[id] = 0;
    }
    else
    {
        pthread_mutex_lock(&counter->localMutexes[id]);
        counter->localValues[id] = 0;
        pthread_mutex_unlock(&counter->localMutexes[id]);
    }
}

void increment(Counter* counter, const int counterId)
{
    pthread_mutex_lock(&counter->localMutexes[counterId]);
    if (++counter->localValues[counterId] == LOCAL_COUNTER_SIZE)
    {
        flush(counter, counterId, true);
    }
    pthread_mutex_unlock(&counter->localMutexes[counterId]);
}

int get(Counter* counter)
{
    pthread_mutex_lock(&counter->globalMutex);
    const int value = counter->globalValue;
    pthread_mutex_unlock(&counter->globalMutex);
    return value;
}

typedef struct _args
{
    Counter* counter;
    int iterations;
    int counterId;
} Args;

void initArgs(Args* args, Counter* counter, const int iterations, const int counterId)
{
    args->counter = counter;
    args->iterations = iterations;
    args->counterId = counterId;
}

void* work(const void* args)
{
    const Args* arg = args;

    const int counterId = arg->counterId;

    for (int i = 0; i < arg->iterations; ++i)
    {
        increment(arg->counter, counterId);
    }

    flush(arg->counter, counterId, false);

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
    const int workPerThread = totalWork / threadAmount;

    Counter counter;
    initCounter(&counter, threadAmount);

    Args* args = malloc(threadAmount * sizeof(Args));

    pthread_t* threads = malloc(threadAmount * sizeof(pthread_t));

    struct timespec firstTime, secondTime, diff;

    clock_gettime(CLOCK_MONOTONIC, &firstTime);

    for (int i = 0; i < threadAmount; ++i)
    {
        int iterations = workPerThread;
        if (i == threadAmount - 1)
        {
            iterations = totalWork - (threadAmount - 1) * workPerThread;
        }
        initArgs(&args[i], &counter, iterations, i);
        pthread_create(&threads[i], NULL, work, &args[i]);
    }

    for (int i = 0; i < threadAmount; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &secondTime);

    getTimeDiff(&firstTime, &secondTime, &diff);

    printf("sec: %lu nsec: %lu  threads: %d work per thread %d total work: %d\n", diff.tv_sec, diff.tv_nsec,
           threadAmount, workPerThread, totalWork);

    assert(get(&counter) == totalWork);

    clearCounter(&counter);
    free(args);
    free(threads);

    return 0;
}
