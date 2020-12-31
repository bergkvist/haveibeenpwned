/**
 * SHA1 is typically represented as a hexadecimal string (40 characters).
 * 
 * When stored as hexadecimal in a plaintext file, it takes up
 * twice as much space as it needs to. A 50% space saving is 
 * especially valuable when the file in question is 25GB.
 */

#ifndef SHA1_COMPRESSION_H
#define SHA1_COMPRESSION_H

#include <stdio.h>
#include <stdint.h>

#define SHA1_UNCOMPRESSED_SIZE 40
#define SHA1_COMPRESSED_SIZE 20

void sha1_decompress(char output[SHA1_UNCOMPRESSED_SIZE], char input[SHA1_COMPRESSED_SIZE]) {
    for (int i = 0; i < 20; i++) {
        sprintf(&output[2*i],   "%01X", (input[i] & 0xF0) >> 4);
        sprintf(&output[2*i+1], "%01X", (input[i] & 0x0F) >> 0);
    }
}

void sha1_compress(char output[SHA1_COMPRESSED_SIZE], char input[SHA1_UNCOMPRESSED_SIZE]) {
    #define HEX_TO_INT4(hex) \
        (((hex) >= '0' && (hex) <= '9') * ((hex) - '0') +\
         ((hex) >= 'A' && (hex) <= 'F') * ((hex) - 'A' + 10))
    for (int i = 0; i < SHA1_COMPRESSED_SIZE; i++) {
        output[i] = ((HEX_TO_INT4(input[2*i])   & 0x0F) << 4)
                  | ((HEX_TO_INT4(input[2*i+1]) & 0x0F) << 0);
    }
    #undef HEX_TO_INT4
}

int sha1_compare_compressed(char h1[SHA1_COMPRESSED_SIZE], char h2[SHA1_COMPRESSED_SIZE]) {
    for (int i = 0; i < SHA1_COMPRESSED_SIZE; i++) {
        if (h1[i] == h2[i]) continue;
        if ((uint8_t)h2[i] > (uint8_t)h1[i]) return -1;
        if ((uint8_t)h2[i] < (uint8_t)h1[i]) return 1;
    }
    return 0;
}

#endif /* SHA1_COMPRESSION_H */