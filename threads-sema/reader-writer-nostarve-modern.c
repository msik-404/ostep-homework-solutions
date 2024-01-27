#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common_threads.h"

//
// Your code goes in the structure and functions below
//

typedef struct __rwlock_t
{
    int is_readers;
    int out_readers;
    bool writer_waiting;
    sem_t write_lock;
    sem_t in_door;
    sem_t out_door;
} rwlock_t;


void rwlock_init(rwlock_t* rw)
{
    rw->is_readers = 0;
    rw->out_readers = 0;
    rw->writer_waiting = false;
    sem_init(&rw->write_lock, 0, 0);
    sem_init(&rw->in_door, 0, 1);
    sem_init(&rw->out_door, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t* rw)
{
    sem_wait(&rw->in_door);
    ++rw->is_readers;
    sem_post(&rw->in_door);
}

void rwlock_release_readlock(rwlock_t* rw)
{
    sem_wait(&rw->out_door);
    ++rw->out_readers;
    if (rw->writer_waiting && rw->out_readers == rw->is_readers)
    {
        sem_post(&rw->write_lock);
    }
    sem_post(&rw->out_door);
}

void rwlock_acquire_writelock(rwlock_t* rw)
{
    sem_wait(&rw->in_door);
    sem_wait(&rw->out_door);
    if (rw->is_readers == rw->out_readers)
    {
        sem_post(&rw->out_door);
    }
    else
    {
        rw->writer_waiting = true;
        sem_post(&rw->out_door);
        sem_wait(&rw->write_lock);
        rw->writer_waiting = false;
    }
}

void rwlock_release_writelock(rwlock_t* rw)
{
    sem_post(&rw->in_door);
}

//
// Don't change the code below (just use it!)
//

int loops;
int value = 0;

rwlock_t lock;

void* reader(void* arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_readlock(&lock);
        printf("read %d\n", value);
        rwlock_release_readlock(&lock);
    }
    return NULL;
}

void* writer(void* arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_writelock(&lock);
        value++;
        printf("write %d\n", value);
        rwlock_release_writelock(&lock);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    assert(argc == 4);
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t pr[num_readers], pw[num_writers];

    rwlock_init(&lock);

    printf("begin\n");

    int i;
    for (i = 0; i < num_readers; i++)
        Pthread_create(&pr[i], NULL, reader, NULL);
    for (i = 0; i < num_writers; i++)
        Pthread_create(&pw[i], NULL, writer, NULL);

    for (i = 0; i < num_readers; i++)
        Pthread_join(pr[i], NULL);
    for (i = 0; i < num_writers; i++)
        Pthread_join(pw[i], NULL);

    printf("end: value %d\n", value);

    return 0;
}
