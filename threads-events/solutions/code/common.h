//
// Created by mateusz on 1/29/24.
//

#ifndef COMMON_H
#define COMMON_H

#include <cstddef>
#include <cstdint>

uint16_t read_port(const char* string);

struct sockaddr_in get_address(const char* host, uint16_t port);

int open_socket();

void bind_socket(int socket_fd, uint16_t port);

void start_listening(int socket_fd, size_t queue_length);

uint16_t get_port(const sockaddr_in* address);

char* get_ip(const sockaddr_in* address);

int accept_connection(int socket_fd, sockaddr_in* client_address);

void connect_socket(int socket_fd, const sockaddr_in* address);

#endif //COMMON_H
