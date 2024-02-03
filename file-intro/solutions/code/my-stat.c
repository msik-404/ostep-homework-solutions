//
// Created by mateusz on 2/2/24.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>

int main(const int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Provide filename.\n");
        exit(1);
    }

    const char* const filename = argv[1];

    struct stat sb;
    if (stat(filename, &sb) == -1)
    {
        fprintf(stderr, "stat syscall failed on file with this filename.\n");
        exit(1);
    }
    printf("ID of containing device:  [%x,%x]\n",
           major(sb.st_dev),
           minor(sb.st_dev));

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT)
    {
    case S_IFBLK: printf("block device\n");
        break;
    case S_IFCHR: printf("character device\n");
        break;
    case S_IFDIR: printf("directory\n");
        break;
    case S_IFIFO: printf("FIFO/pipe\n");
        break;
    case S_IFLNK: printf("symlink\n");
        break;
    case S_IFREG: printf("regular file\n");
        break;
    case S_IFSOCK: printf("socket\n");
        break;
    default: printf("unknown?\n");
        break;
    }

    printf("I-node number:            %ju\n", sb.st_ino);

    printf("Mode:                     %jo (octal)\n", (uintmax_t)sb.st_mode);

    printf("Link count:               %ju\n", sb.st_nlink);
    printf("Ownership:                UID=%ju   GID=%ju\n", (uintmax_t)sb.st_uid, (uintmax_t)sb.st_gid);

    printf("Preferred I/O block size: %jd bytes\n", sb.st_blksize);
    printf("File size:                %jd bytes\n", sb.st_size);
    printf("Blocks allocated:         %jd\n", sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));

    return 0;
}
