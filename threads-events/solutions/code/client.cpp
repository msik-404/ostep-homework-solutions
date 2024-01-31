//
// Created by mateusz on 1/29/24.
//

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE (4 + 2048) // first four bytes is the message length in bytes, smaller or equal to 2048 - 4
#define HEADER_SIZE sizeof(uint32_t) // header constains amout of bytes in the message.

char shared_buffer[BUFFER_SIZE];

void clean(const int socket_fd, char* buffer)
{
    // Graceful disconnection. - FIN
    CHECK_ERRNO(shutdown(socket_fd, SHUT_WR));
    // Forece disconnection. - RST
    CHECK_ERRNO(close(socket_fd));
}

int main(const int argc, char* argv[])
{
    if (argc < 3)
    {
        fatal("Usage: %s host port message ...\n", argv[0]);
    }

    const char* host = argv[1];
    const uint16_t port = read_port(argv[2]);
    const struct sockaddr_in server_address = get_address(host, port);

    const int socket_fd = open_socket();

    connect_socket(socket_fd, &server_address);

    char buffer[BUFFER_SIZE];
    ENSURE(buffer != nullptr);

    char* const header_start = buffer;
    char* const content_start = header_start + HEADER_SIZE;
    constexpr size_t max_content_size = BUFFER_SIZE - HEADER_SIZE;

    while (true)
    {
        // clean buffer.
        memset(buffer, 0, BUFFER_SIZE);
        // set data in data part of the buffer.
        ssize_t count = 0;
        while (count < max_content_size)
        {
            const int c = fgetc(stdin);
            if (c == 10 || c == EOF)
            {
                break;
            }
            *(content_start + count) = static_cast<char>(c);
            count++;
        }
        if (strcmp(content_start, "EXIT") == 0)
        {
            break;
        }
        // make count network order and set it in count part of the buffer.
        uint32_t count_net = htonl(count);
        memcpy(header_start, &count_net, HEADER_SIZE);
        // This is the same but using pointers:
        // *(uint32_t*) buffer = count_net;

        count = write(socket_fd, buffer, count + HEADER_SIZE);
        if (count < 0)
        {
            clean(socket_fd, buffer);
            PRINT_ERRNO();
        }
    }

    clean(socket_fd, buffer);

    return 0;
}
