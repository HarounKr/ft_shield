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

    size_t key_len = strlen((char *)HASHED_PWD);
    unsigned char output_bytes[key_len / 2];

    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    hextobin(output_bytes, (unsigned char *)HASHED_PWD, key_len / 2);
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    if (!ctx)
        return false;

    if (!EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) || 
        !EVP_DigestUpdate(ctx, passwd, passwd_len) || 
        !EVP_DigestUpdate(ctx, SALT, strlen(SALT)) ||
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