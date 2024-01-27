#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct node
{
    sem_t waiter_mutex;
    struct node* next;
} node_t;

void node_init(node_t* node)
{
    sem_init(&node->waiter_mutex, 0, 0);
    node->next = NULL;
}

void node_free(node_t* node)
{
    sem_destroy(&node->waiter_mutex);
    free(node);
}

typedef struct wait_queue
{
    node_t* root;
    node_t* tail;
} wait_queue_t;

void list_init(wait_queue_t* l)
{
    l->root = NULL;
    l->tail = NULL;
}

void list_free(wait_queue_t* l)
{
    while (l->root != NULL)
    {
        node_t* node = l->root;
        l->root = node->next;
        node_free(node);
    }
}

node_t* insert_and_wait(wait_queue_t* l)
{
    node_t* node = malloc(sizeof(node));
    node_init(node);
    if (l->root == NULL)
    {
        l->root = node;
        l->tail = node;
    }
    else
    {
        l->tail->next = node;
        l->tail = node;
    }
    return node;
}

bool pop_and_post(wait_queue_t* l)
{
    if (l->root == NULL)
    {
        return true;
    }

    node_t* node = l->root;
    l->root = node->next;
    sem_post(&node->waiter_mutex);

    return false;
}

typedef struct __ns_mutex_t
{
    wait_queue_t wait_list;

    bool is_free;
    sem_t mutex;

    sem_t internal_mutex;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t* m)
{
    list_init(&m->wait_list);
    m->is_free = true;

    sem_init(&m->mutex, 0, 1);
    sem_init(&m->internal_mutex, 0, 1);
}

void ns_mutex_free(ns_mutex_t* m)
{
    sem_destroy(&m->mutex);
    sem_destroy(&m->internal_mutex);
    list_free(&m->wait_list);
}

void ns_mutex_acquire(ns_mutex_t* m)
{
    sem_wait(&m->internal_mutex);
    if (m->is_free)
    {
        m->is_free = false;
        sem_post(&m->internal_mutex);
        sem_wait(&m->mutex);
    }
    else
    {
        node_t* waiter_node = insert_and_wait(&m->wait_list);
        sem_post(&m->internal_mutex);
        sem_wait(&waiter_node->waiter_mutex);
        node_free(waiter_node);
        // Woken thread should free it's node's memory and destroy it's semaphore.
        // Prior to this solution, waking thread was doing this work and it resultated in errors.
        // Waking thread first signaled semaphore and then removed it before sleeping thread could notice waking.
    }
}

void ns_mutex_release(ns_mutex_t* m)
{
    sem_wait(&m->internal_mutex);
    m->is_free = pop_and_post(&m->wait_list); // false if someone was woken.
    if (m->is_free)
    {
        sem_post(&m->mutex);
    }
    sem_post(&m->internal_mutex);
}

typedef struct args
{
    ns_mutex_t* ns_mutex;
    int thread_idx;
    int loops;
} args_t;

void args_init(args_t* args, ns_mutex_t* mutex, const int threadIdx, const int loops)
{
    args->ns_mutex = mutex;
    args->thread_idx = threadIdx;
    args->loops = loops;
}

void* worker(void* arg)
{
    const args_t* args = arg;
    for (int i = 0; i < args->loops; ++i)
    {
        ns_mutex_acquire(args->ns_mutex);
        printf("Hello there, iteration: %d by worker_%d\n", i, args->thread_idx);
        ns_mutex_release(args->ns_mutex);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    assert(argc == 3);
    const int num_threads = atoi(argv[1]);
    assert(num_threads > 0);
    const int loops = atoi(argv[2]);

    printf("parent: begin\n");

    pthread_t p[num_threads];

    ns_mutex_t ns_mutex;
    ns_mutex_init(&ns_mutex);

    args_t a[num_threads];
    for (int i = 0; i < num_threads; ++i)
    {
        args_init(&a[i], &ns_mutex, i, loops);
        Pthread_create(&p[i], NULL, worker, &a[i]);
    }
    for (int i = 0; i < num_threads; ++i)
    {
        Pthread_join(p[i], NULL);
    }

    ns_mutex_free(&ns_mutex);

    printf("parent: end\n");
    return 0;
}
