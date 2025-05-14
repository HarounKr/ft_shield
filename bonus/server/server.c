#include "gnl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // For open()
#include <signal.h> // For handling interruptions
#include <errno.h>  // For error handling

#define PORT 4242

int server_fd = -1;

void handle_sigint(int sig) {
    (void)sig;
    if (server_fd != -1) {
        close(server_fd);
        printf("\nServer closed.\n");
    }
    exit(0);
}

int main() {
    struct sockaddr_in address;
    int opt = 1;
    char buffer[1024] = {0};
    socklen_t addrlen = sizeof(address);
    int new_socket, log_fd;

    signal(SIGINT, handle_sigint);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Loop to continuously accept connections
    while (1) {
        // Accept new connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket < 0) {
            // If accept fails, print the error and continue accepting new connections
            if (errno == EINTR) {
                // Interrupted system call, can ignore
                continue;
            } else {
                perror("accept failed");
                continue; // Do not close server, keep trying to accept new connections
            }
        }

        // Only open the file if the connection was successful
        log_fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (log_fd == -1) {
            perror("Error opening log.txt");
            close(new_socket);
            continue; // Continue to accept next connection if log file can't be opened
        }

        printf("Connection established, storing data in log.txt...\n");

        // Read data from the client and write it to the log file
        while (1) {
            int valread = read(new_socket, buffer, sizeof(buffer));
            if (valread <= 0) {
                break; // Connection closed or error occurred, break out of the loop
            } else {
                buffer[valread] = '\0'; // Null-terminate the string
                printf("Received key: %s\n", buffer);

                // Write the received data to log.txt
                ssize_t bytes_written = write(log_fd, buffer, valread);
                if (bytes_written == -1) {
                    perror("Error writing to log.txt");
                    break; // Break if there's an issue writing to the file
                }
            }
        }

        // Close the log file and the connection
        close(log_fd);
        close(new_socket);
        printf("Connection closed, log.txt saved.\n");
    }

    // Close the server socket when exiting
    close(server_fd);
    return 0;
}
