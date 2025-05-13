#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4242

void sighandler(int sig ) {
    if (sig == SIGINT) {
        printf("ok\n");
    }
}

SSL_CTX *create_context() {
    const SSL_METHOD *ssl_method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(ssl_method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_load_verify_file(ctx, "/tmp/ft_shield_cert.pem") <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

int create_socket() {
    int client_sock;
    struct sockaddr_in server_addr;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_sock);
        exit(1);
    }
    return client_sock;
}

void log_with_timestamp(char *message, const char *action, size_t bytes, FILE *logfile) {
    time_t now;
    char timeStr[20];
    struct tm *timeinfo;
    //message[strcspn(message, "\n")] = 0;
    time(&now);
    timeinfo = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
    fprintf(logfile, "[%s] %s %s [%zu octets]\n", timeStr, action, message, bytes);
}

int main() {
    fd_set readfds;
    char buffer[1024];
    int client_sock = create_socket();
    SSL_CTX *ctx = create_context();
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_sock);
    SSL_connect(ssl);

    signal(SIGINT, sighandler);
    FILE *logfile = fopen("data.log", "wb");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);     // stdin
        FD_SET(client_sock, &readfds);  // socket
        memset(buffer, 0, 1024);

        int maxfd = 0;
        if (client_sock > maxfd) {
            maxfd = client_sock + 1;
        }
        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // Surveiller la saisie utilisateur
        if (FD_ISSET(0, &readfds)) {
            ssize_t len = read(0, buffer, sizeof(buffer));
            if (len <= 0)
                break;
            buffer[len] = '\0';
            SSL_write(ssl, buffer, len);
            log_with_timestamp(buffer, "SEND" ,len, logfile);
        }

        // Surveiller le socket
        if (FD_ISSET(client_sock, &readfds)) {
            size_t readbytes;
            int ret = SSL_read_ex(ssl, buffer, 1024, &readbytes);
            if (readbytes <= 0 || !ret)
                break;
            buffer[readbytes] = '\0';
            write(1, buffer, readbytes);
            log_with_timestamp(buffer, "RECEIVED" ,readbytes, logfile);
        }
    }
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    fclose(logfile);
    close(client_sock);
    return 0;
}
