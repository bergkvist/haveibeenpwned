#include <stdio.h>
#include <openssl/sha.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <PASSWORD>\n", argv[0]);
        return 1;
    }
    char *password = argv[1];
    char *hash = SHA1(password, strlen(password), NULL);
    for (int i = 0; i < 20; i++) {
        printf("%02X", hash[i] & 0xFF);
    }
}