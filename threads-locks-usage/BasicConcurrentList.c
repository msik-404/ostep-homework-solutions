//
// Created by mateusz on 1/20/24.
//

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _node
{
    int value;
    struct _node* next;
} Node;

typedef struct _list
{
    pthread_mutex_t mutex;
    Node* head;
} List;

void init(List* list)
{
    pthread_mutex_init(&list->mutex, NULL);
    list->head = NULL;
}

void insert(List* list, const int value)
{
    pthread_mutex_lock(&list->mutex);
    Node* newNode = malloc(1 * sizeof(Node));
    newNode->value = value;
    newNode->next = list->head;
    list->head = newNode;
    pthread_mutex_unlock(&list->mutex);
}

int _peek(List* list, const bool isWithinMutex)
{
    if (isWithinMutex)
    {
        assert(list->head != NULL);
        return list->head->value;
    }
    pthread_mutex_lock(&list->mutex);
    assert(list->head != NULL);
    const int output = list->head->value;
    pthread_mutex_unlock(&list->mutex);
    return output;
}

int pop(List* list)
{
    pthread_mutex_lock(&list->mutex);
    const int output = _peek(list, true);
    Node* next = list->head->next;
    free(list->head);
    list->head = next;
    pthread_mutex_unlock(&list->mutex);
    return output;
}

void clean(List* list)
{
    pthread_mutex_lock(&list->mutex);
    Node* curr = list->head;
    while (curr != NULL)
    {
        Node* next = curr->next;
        free(curr);
        curr = next;
    }
    pthread_mutex_unlock(&list->mutex);
}

typedef struct _args
{
    List* list;
    int iterations;
    int counterId;
} Args;

void initArgs(Args* args, List* list, const int iterations)
{
    args->list = list;
    args->iterations = iterations;
}

void* work(const void* args)
{
    const Args* arg = args;

    for (int i = 0; i < arg->iterations; ++i)
    {
        insert(arg->list, 1);
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
    const int workPerThread = totalWork / threadAmount;

    List list;
    init(&list);

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
        initArgs(&args[i], &list, iterations);
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

    int sum = 0;
    for (int i = 0; i < totalWork; ++i)
    {
        sum += pop(&list);
    }

    assert(sum == totalWork);

    clean(&list);
    free(args);
    free(threads);

    return 0;
}
