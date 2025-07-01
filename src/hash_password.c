#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>



int main() {
    const char *password = "Lemdpestsecure42!";
    const char *salt = "4j9K2x7VzQmLp3Bt";
  

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, password, strlen(password));
    EVP_DigestUpdate(ctx, salt, strlen(salt));
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
      
    EVP_MD_CTX_free(ctx);
    

    for (unsigned int i = 0; i < hash_len; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
    return 0;
}
