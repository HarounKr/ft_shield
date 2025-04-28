#include "trojan.h"

void ft_shutdown(t_args *args) {
    SSL_shutdown(args->ssl);
    SSL_free(args->ssl);
    close(args->client_socket);
    free(args);
}