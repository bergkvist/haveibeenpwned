
#include "sha1-compression.h"
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <INDEX> <SHA1-FILE>\n", argv[0]);
        return 1;
    }
    size_t index = atoi(argv[1]);
    char *filename = argv[2];
    
    char compressed_hash[SHA1_COMPRESSED_SIZE];
    FILE *f = fopen(filename, "rb");
    fseek(f, (size_t)SHA1_COMPRESSED_SIZE * index, SEEK_SET);
    fread(&compressed_hash, sizeof(char), SHA1_COMPRESSED_SIZE, f);
    fclose(f);

    char uncompressed_hash[SHA1_UNCOMPRESSED_SIZE+1];
    uncompressed_hash[SHA1_UNCOMPRESSED_SIZE] = '\0';
    sha1_decompress(&uncompressed_hash[0], &compressed_hash[0]);
    printf("%s", uncompressed_hash);
}