//
// Created by mateusz on 2/2/24.
//

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define D_NAME_SIZE 256
#define INIT_PATH_BUFFER_SIZE 32

void increase_path_buffer_capacity(char* path_buffer, int* const buffer_capacity)
{
    const int new_max_path_size = *buffer_capacity * 1.5;
    path_buffer = realloc(path_buffer, new_max_path_size);
    assert(path_buffer != NULL);
    *buffer_capacity = new_max_path_size;
}

int add_to_path(char* path_buffer, const int path_size, int* const buffer_capacity, const char* const path_suffix)
{
    const int path_suffix_size = strlen(path_suffix);
    if (path_size + path_suffix_size + 1 > *buffer_capacity) // + 1 for null terminator.
    {
        increase_path_buffer_capacity(path_buffer, buffer_capacity);
    }
    strcpy(path_buffer + path_size, path_suffix);
    return path_size + path_suffix_size;
}

void dfs(char* path, int curr_path_size, int* const max_path_size)
{
    curr_path_size = add_to_path(path, curr_path_size, max_path_size, "/");

    DIR* dp = opendir(path);
    if (dp == NULL)
    {
        return;
    }

    struct dirent* d;
    while ((d = readdir(dp)) != NULL)
    {
        const char* const file = d->d_name;
        if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
        {
            continue;
        }
        const int new_path_size = add_to_path(path, curr_path_size, max_path_size, file);
        printf("%s\n", path);
        dfs(path, new_path_size, max_path_size);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Missing input path.\n");
        exit(1);
    }

    const char* const path = argv[1];

    int init_buffer_capacity = INIT_PATH_BUFFER_SIZE * D_NAME_SIZE;
    char* const start_path = malloc(init_buffer_capacity * sizeof(char));
    strcpy(start_path, path);
    const int path_size = strlen(start_path);

    dfs(start_path, path_size, &init_buffer_capacity);

    free(start_path);

    return 0;
}
