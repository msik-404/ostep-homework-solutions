//
// Created by mateusz on 1/20/24.
//

#include <pthread.h>
#include <stdlib.h>

typedef struct _node
{
    int value;
    struct _node* next;
    pthread_mutex_t mutex;
} Node;

void initNode(Node* node, const int value, Node* next)
{
    node->value = value;
    pthread_mutex_init(&node->mutex, NULL);
    node->next = next;
}

void clean(Node* node)
{
    pthread_mutex_destroy(&node->mutex);
}

void lock(Node* node)
{
    pthread_mutex_lock(&node->mutex);
}

void unlock(Node* node)
{
    pthread_mutex_unlock(&node->mutex);
}

typedef struct _list
{
    Node* root;
} List;

void initList(List* list)
{
    list->root = NULL;
}

void clean(List* list)
{
    Node* curr = list->root;
    while (curr != NULL)
    {
        Node* next = curr->next;
        clean(curr);
        free(curr);
        curr = next;
    }
}

void insert(List* list, const int value)
{
    lock(list->root);

    Node* newRoot = malloc(sizeof(Node));
    initNode(newRoot, value, list->root);

    lock(newRoot);
    list->root = newRoot;

    unlock(list->root);
    unlock(list->root->next);
}

int peek(const List* list, const bool isLockHold)
{
    if (isLockHold)
    {
        return list->root->value;
    }
    lock(list->root);
    const int value = list->root->value;
    unlock(list->root);
    return value;
}

int pop(List* list)
{
    lock(list->root);

    const int value = list->root->value;

    lock(list->root->next);
    Node* oldRoot = list->root;
    list->root = list->root->next;

    unlock(oldRoot);
    clean(oldRoot);
    free(oldRoot);

    unlock(list->root);
    return value;
}

int constains(const List* list, const int value)
{
    int i = 0;
    Node* curr = list->root;
    Node* prev = NULL;

    while (curr != NULL)
    {
        lock(curr);
        if (i != 0)
        {
            unlock(prev);
        }
        if (curr->value == value)
        {
            unlock(curr);
            return i;
        }
        prev = curr;
        curr = curr->next;
        i++;
    }

    return -1;
}

int updateValue(const List* list, const int oldValue, const int newValue)
{
    int i = 0;
    Node* curr = list->root;
    Node* prev = NULL;

    while (curr != NULL)
    {
        lock(curr);
        if (i != 0)
        {
            unlock(prev);
        }
        if (curr->value == oldValue)
        {
            curr->value = newValue;
            unlock(curr);
            return i;
        }
        prev = curr;
        curr = curr->next;
        i++;
    }

    return -1;
}

int updateByIndex(const List* list, const int index, const int newValue)
{
    int i = 0;
    Node* curr = list->root;
    Node* prev = NULL;

    while (curr != NULL)
    {
        lock(curr);
        if (i != 0)
        {
            unlock(prev);
        }
        if (i == index)
        {
            curr->value = newValue;
            unlock(curr);
            return i;
        }
        prev = curr;
        curr = curr->next;
        i++;
    }

    return -1;
}
