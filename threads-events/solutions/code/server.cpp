//
// Created by mateusz on 1/29/24.
//

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "common.h"
#include "err.h"

#define BUFFER_SIZE 2048 // Max message size.
#define HEADER_SIZE sizeof(uint32_t) // Header constains amout of bytes in the message.
#define QUEUE_LENGTH 32
#define MAX_EVENT_AMOUNT 64

/*
 * register events of fd to epoll_fd
 */
static void epoll_ctl_add(const int epoll_fd, const int fd, const uint32_t events)
{
	epoll_event ev{};
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_ctl()\n");
		exit(1);
	}
}

int main(const int argc, char* argv[])
{
    if (argc != 2)
    {
        fatal("Usage: %s <port>", argv[0]);
    }

    const uint16_t port = read_port(argv[1]);

    // Buffer for client sent data.
    char* buffer[BUFFER_SIZE];

    const int epoll_fd = epoll_create(0);
    if (epoll_fd < 0)
    {
        PRINT_ERRNO();
    }

    const int listen_sock = open_socket();

    start_listening(listen_sock, QUEUE_LENGTH);

    printf("Listening on port %u\n", port);

    epoll_ctl_add(epoll_fd, listen_sock, EPOLLIN);

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);

		epoll_event events[MAX_EVENT_AMOUNT];
        const int nfds = epoll_wait(epoll_fd, events, MAX_EVENT_AMOUNT, -1);

        for (int i = 0; i < nfds; ++i)
        {
        	/* New connection */
            if (events[i].data.fd == listen_sock)
            {
                sockaddr_in client_address{};

                const int conn_sock = accept_connection(listen_sock, &client_address);
                if (conn_sock < 0)
                {
                    PRINT_ERRNO();
                }

                epoll_ctl_add(epoll_fd, conn_sock, EPOLLIN | EPOLLRDHUP | EPOLLHUP);

                const char* client_ip = get_ip(&client_address);
                const uint16_t client_port = get_port(&client_address);
                printf("Accepted connection from %s:%d\n", client_ip, client_port);
            } else if (events[i].events & EPOLLIN) // Read from client.
            {
            	/*
            	 * TODO: Create some global hashmap fd -> buffer to not block on read if not enough data came.
            	 *
            	 * Each connected user has his struct with uint32_t bytes to read and buffer for the data to collect.
            	 * Once the data is collected response should be generated.
            	 *
            	 * I guess once the user's buffer is filled, write event should be placed in the epoll, and server should
            	 * try to write the response. In the mean time if client tries to write more data it should be collected
            	 * in addiotnal buffer. This generated additional how many additional buffers we need.
            	 *
            	 * Maybe we should just remove EPOLLIN event and just keep EPOLLOUT and events for closing connection.
            	 * I would have to do some more research if this sollution is possible.
				 */
            	uint32_t bytes_to_read;
            	ssize_t count = read(i, &bytes_to_read, sizeof(uint32_t));
            	if (count != 4)
            	{
            		fatal("Could not read header. Count: %ld", count);
            	}
            	bytes_to_read = ntohl(bytes_to_read);
				count = read(i, &buffer, bytes_to_read);
            	if (count != bytes_to_read)
            	{
            		fatal("Could not read specified data. Count: %ld requresed amount: %d", count, bytes_to_read);
            	}

            }

            /* Check if the connection is closing */
			if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
				printf("Connection closed\n");
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
				close(events[i].data.fd);
			}
        }
    }

    CHECK(close(listen_sock));

    CHECK(close(epoll_fd));

    return 0;
}

// uint32_t rev;
// memcpy(&rev, buffer, sizeof(uint32_t));
// uint32_t rev = ntohl(*(uint32_t*) buffer);
// write filled buffer part to the socket
