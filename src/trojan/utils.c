#include "trojan.h"

void ft_shutdown(t_args *args) {
    SSL_shutdown(args->ssl);
    SSL_free(args->ssl);
    close(args->client_socket);
    free(args);
}

void format_response(char *dest, char *src) {
    memset(dest, 0, 1024);
    snprintf(dest, 1024, "%s", src);
}
