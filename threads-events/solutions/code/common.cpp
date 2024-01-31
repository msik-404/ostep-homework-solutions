//
// Created by mateusz on 1/29/24.
//

#include "common.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "err.h"

uint16_t read_port(const char* string)
{
    errno = 0;
    const unsigned long port = strtoul(string, nullptr, 10);
    PRINT_ERRNO();
    if (port > UINT16_MAX)
    {
        fatal("%u is not a valid port number", port);
    }
    return port;
}

sockaddr_in get_address(const char* host, const uint16_t port)
{
    addrinfo hints{};
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* address_result;
    CHECK(getaddrinfo(host, nullptr, &hints, &address_result));

    sockaddr_in address{};
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = reinterpret_cast<sockaddr_in*>(address_result->ai_addr)->sin_addr.s_addr;
    address.sin_port = htons(port);

    freeaddrinfo(address_result);

    return address;
}

int open_socket()
{
    const int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0)
    {
        PRINT_ERRNO();
    }
    return socket_fd;
}

void bind_socket(const int socket_fd, const uint16_t port)
{
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    CHECK_ERRNO(bind(socket_fd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)));
}

void start_listening(const int socket_fd, const size_t queue_length)
{
    CHECK_ERRNO(listen(socket_fd, queue_length));
}

uint16_t get_port(const sockaddr_in* address)
{
    return ntohs(address->sin_port);
}

char* get_ip(const sockaddr_in* address)
{
    return inet_ntoa(address->sin_addr);
}

int accept_connection(const int socket_fd, sockaddr_in* client_address)
{
    socklen_t client_address_length = sizeof(client_address);

    const int client_fd = accept(socket_fd, reinterpret_cast<sockaddr*>(client_address), &client_address_length);
    if (client_fd < 0)
    {
        PRINT_ERRNO();
    }

    return client_fd;
}

void connect_socket(const int socket_fd, sockaddr_in* address)
{
    CHECK_ERRNO(connect(socket_fd, reinterpret_cast<sockaddr*>(address), sizeof(*address)));
}
