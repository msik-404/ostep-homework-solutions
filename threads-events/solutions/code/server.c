//
// Created by mateusz on 1/29/24.
//

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common.h"
#include "err.h"

#define BUFFER_SIZE 40
#define QUEUE_LENGTH 5

int main(const int argc, char* argv[])
{
    if (argc != 2)
    {
        fatal("Usage: %s <port>", argv[0]);
    }

    const uint16_t port = read_port(argv[1]);

    const int socket_fd = open_socket();

    start_listening(socket_fd, QUEUE_LENGTH);

    printf("Listening on port %u\n", port);

    char buffer[BUFFER_SIZE];

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        struct sockaddr_in client_address;

        const int client_fd = accept_connection(socket_fd, &client_address);

        const char* client_ip = get_ip(&client_address);
        const uint16_t client_port = get_port(&client_address);
        printf("Accepted connection from %s:%d\n", client_ip, client_port);

        size_t read_length;
        do
        {
        } while (read_length > 0);
        printf("Closing connection\n");
        CHECK(close(client_fd));
    }

    CHECK(close(socket_fd));

    return 0;
}
