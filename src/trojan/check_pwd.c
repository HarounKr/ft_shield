#include "trojan.h"

// https://stackoverflow.com/questions/2262386/generate-sha256-with-openssl-and-c

void hextobin(unsigned char *output_bytes, const unsigned char *hex_str, size_t n) {
    char hex_pair[3];  // deux caractères hexadécimaux + \0
    const unsigned char *current_char = hex_str;

    for (size_t i = 0; i < n; ++i) {
        memcpy(hex_pair, current_char, 2);
        hex_pair[2] = '\0';
        output_bytes[i] = (unsigned char)strtol(hex_pair, NULL, 16);
        current_char += 2;
    }
}

bool check_pwd(unsigned char *passwd, size_t passwd_len) {
    const char *salt = "4j9K2x7VzQmLp3Bt";
    // char *key = "2f811d7573e415e52f7892298226d13b6f999463fb04267c9cbf8e1bda663660";
    char *key = "4ba117368c491a5d90b64fcccd7d2cedb31146545e87d659cbd0d5cacb3f3064";

    size_t key_len = strlen((char *)key);
    unsigned char output_bytes[key_len / 2];

    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    hextobin(output_bytes, (unsigned char *)key, key_len / 2);
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    if (!ctx)
        return false;

    if (!EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) || 
        !EVP_DigestUpdate(ctx, passwd, passwd_len) || 
        !EVP_DigestUpdate(ctx, salt, strlen(salt)) ||
        !EVP_DigestFinal_ex(ctx, md, &md_len)) {
        
        EVP_MD_CTX_free(ctx);
        return false;
    }
    EVP_MD_CTX_free(ctx);
    
    if ((key_len / 2) == (md_len) && memcmp(md, output_bytes, md_len) == 0) {
        /*for (unsigned int i = 0; i < md_len; i++) {
            printf("%02x", md[i]);
        }
        printf("\n");
        for (unsigned int i = 0; i < md_len; i++) {
            printf("%02x", output_bytes[i]);
        }
        printf("\n"); */ 
        return true;
    }
   
    return false;
}