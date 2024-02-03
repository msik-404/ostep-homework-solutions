//
// Created by mateusz on 2/2/24.
//

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INIT_BUFF_SIZE 4096
#define STEP_SIZE 16 * sizeof(char)


int main(const int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Provide filename.\n");
        exit(1);
    }

    const char* const filename = argv[2];
    const int n = atoi(argv[1]);
    if (n == 0)
    {
        fprintf(stderr, "Could not transform stirng: %s to int", argv[1]);
        exit(1);
    }

    const int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open the file.\n");
        exit(1);
    }

    int buff_size = INIT_BUFF_SIZE;
    char* buff = malloc(buff_size * STEP_SIZE);
    assert(buff != NULL);

    bool finished = false;
    int lines_read = 0;
    int fetch_idx = 0;
    int read_amount = 0;
    int fetch_size = STEP_SIZE;
    int last_fetch_offset = 0;
    int last_fetch_size = 0;
    off_t file_offset = lseek(fd, -STEP_SIZE, SEEK_END);
    do
    {
        off_t prev_file_offset = file_offset;

        if (fetch_idx > 0)
        {
            prev_file_offset = file_offset;
            file_offset = lseek(fd, prev_file_offset - STEP_SIZE, SEEK_SET);
            if (file_offset == -1)
            {
                lseek(fd, 0, SEEK_SET);
                fetch_size = prev_file_offset;
            }
        }
        read_amount = read(fd, buff + fetch_idx * STEP_SIZE, fetch_size);
        if (read_amount == -1)
        {
            fprintf(stderr, "Could not read the data.\n");
            exit(1);
        }

        last_fetch_size = read_amount;
        for (int i = 0; i < read_amount; ++i)
        {
            // start from the end of current fetch.
            const char c = buff[fetch_idx * STEP_SIZE + (read_amount - 1) - i];
            if (c == '\n')
            {
                ++lines_read;
                if (lines_read == n + 1)
                {
                    last_fetch_offset = read_amount - i;
                    last_fetch_size = i;
                    finished = true;
                    break;
                }
            }
        }
        fetch_idx++;
        if (fetch_idx == buff_size)
        {
            buff_size = buff_size * 2;
            buff = realloc(buff, buff_size * STEP_SIZE);
            assert(buff != NULL);
        }
    } while (file_offset != -1 && !finished);

    for (int j = fetch_idx - 1; j >= 0; --j)
    {
        int write_amount = STEP_SIZE;
        int curr_offset = 0;
        if (j == fetch_idx - 1)
        {
            curr_offset = last_fetch_offset;
            write_amount = last_fetch_size;
        }
        read_amount = write(STDOUT_FILENO, buff + j * STEP_SIZE + curr_offset, write_amount);
        if (read_amount == -1)
        {
            fprintf(stderr, "Could not write the data.\n");
            exit(1);
        }
    }

    close(fd);
    free(buff);

    return 0;
}
