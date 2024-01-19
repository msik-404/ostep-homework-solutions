#include <stdio.h>

#include "common_threads.h"

int balance = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* worker(void* arg)
{
    int rc = pthread_mutex_lock(&lock);
    assert(rc == 0);

    balance++; // unprotected access

    rc = pthread_mutex_unlock(&lock);
    assert(rc == 0);

    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t p;
    Pthread_create(&p, NULL, worker, NULL);
    int rc = pthread_mutex_lock(&lock);
    assert(rc == 0);

    balance++; // unprotected access

    rc = pthread_mutex_unlock(&lock);
    assert(rc == 0);

    Pthread_join(p, NULL);
    return 0;
}
