//
// Created by mateusz on 2/2/24.
//

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define D_NAME_SIZE 256

int main(const int argc, char* argv[])
{
    bool use_stat = false;
    char buff[2];
    strcpy(buff, ".\0");
    const char* dir_path = buff;

    if (argc > 1 && strcmp(argv[1], "-l") == 0)
    {
        use_stat = true;
    }

    if (argc == 3)
    {
        dir_path = argv[2];
    }

    int dir_path_size = strlen(dir_path);

    int max_realative_path_size = dir_path_size + D_NAME_SIZE;
    char relative_path[max_realative_path_size];

    strcpy(relative_path, dir_path);
    relative_path[dir_path_size++] = '/';

    DIR* dp = opendir(dir_path);
    if (dp == NULL)
    {
        fprintf(stderr, "Provided directory does not exist.\n");
        exit(1);
    }
    struct dirent* d;
    while ((d = readdir(dp)) != NULL)
    {
        printf("%s", d->d_name);
        if (use_stat)
        {
            strcpy(relative_path + dir_path_size, d->d_name);
            struct stat sb;
            assert(stat(relative_path, &sb) != -1);

            printf(" | UID=%ju | GID=%ju", (uintmax_t)sb.st_uid, (uintmax_t)sb.st_gid);
            printf(" | Mode=%jo", (uintmax_t)sb.st_mode);
            printf(" | I-number=%ju", sb.st_ino);
        }
        printf("\n");
    }

    closedir(dp);
    return 0;
}
