#include <stdio.h>
#include <string.h>

int main() {
    char target[] = "pthread_detach";
    int test[] = {90, 94, 66, 88, 79, 75, 78, 117, 78, 79, 94, 75, 73, 66};
    char encoded[50];
    char decoded[50];
    int key = 42;
    for (int i = 0; i < strlen(target); i++) {
        encoded[i] = target[i] ^ key;
        printf("%d ", encoded[i]);
    }
    encoded[strlen(target)] = '\0';

    for (int i = 0; i < strlen(encoded); i++) {
        decoded[i] = encoded[i] ^ key;
    }
    printf("len : %zu\n", strlen(decoded));

    printf("%s\n", decoded);

    if (strcmp(decoded, target) == 0)
        printf("Key %d decrypts the encoded string to 'Congratulations!'\n", key);
    else
        printf("Key %d does not decrypt correctly.\n", key);
}