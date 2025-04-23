#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4242

void show_prompt() {
    write(1, "$ ", 3);
}

void sighandler(int sig ) {
    if (sig == SIGINT) {
        printf("ok\n");
    }
}

int main() {
    int client_sock;
    struct sockaddr_in server_addr;
    fd_set readfds;
    char buffer[1024];

    // Création socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_sock);
        exit(1);
    }

    show_prompt();
    signal(SIGINT, sighandler);
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);     // stdin
        FD_SET(client_sock, &readfds);  // socket

        int maxfd;
        
        if (client_sock > 0) {
            maxfd = client_sock + 1;
        } else {
            maxfd = 1;
        }
        if (select(maxfd, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // Surveiller la saisie utilisateur
        if (FD_ISSET(0, &readfds)) {
            ssize_t len = read(0, buffer, sizeof(buffer));
            if (len <= 0)
                break;
            send(client_sock, buffer, len, 0);
        }

        // Surveiller le socket
        if (FD_ISSET(client_sock, &readfds)) {
            ssize_t len = read(client_sock, buffer, sizeof(buffer));
            if (len <= 0)
                break;
            write(1, buffer, len);
            show_prompt();
        }
    }

    close(client_sock);
    return 0;
}
