//
// Created by mateusz on 1/29/24.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE 40

char shared_buffer[BUFFER_SIZE];

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

    const char* server_ip = get_ip(&server_address);
    const uint16_t server_port = get_port(&server_address);

    // Graceful disconnection. - FIN
    CHECK_ERRNO(shutdown(socket_fd, SHUT_WR));

    // Forece disconnection. - RST
    CHECK_ERRNO(close(socket_fd));

    return 0;
}
