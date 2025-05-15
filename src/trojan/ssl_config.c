#include "trojan.h"

SSL_CTX *create_context() {
    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        // ERR_print_errors_fp(stderr);
        sc(SYS_exit, EXIT_FAILURE);
    }
    return ctx;
}

void configure_cert(SSL_CTX *ctx) {
    if (SSL_CTX_use_certificate_file(ctx, SSL_CERT_PATH, SSL_FILETYPE_PEM) <= 0) {
        // ERR_print_errors_fp(stderr);
        sc(SYS_exit, EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, SSL_KEY_PATH, SSL_FILETYPE_PEM) <= 0) {
        // ERR_print_errors_fp(stderr);
        sc(SYS_exit, EXIT_FAILURE);
    }
}