#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/time.h>
#include <locale.h>
#include "sha1-compression.h"

#define INPUT_CHUNK_SIZE 1024
#define MAX_COUNT_DIGITS 16
#define OUTPUT_HASH_BUFFER_LENGTH (1024 * SHA1_COMPRESSED_SIZE)
#define OUTPUT_COUNT_BUFFER_LENGTH 1024

struct input_buffer_s {
    FILE *file;
    bool is_reading_hash;
    uint64_t hash_cursor;
    uint64_t count_cursor;
    uint64_t buffer_cursor;
    char hash[SHA1_UNCOMPRESSED_SIZE+1];
    char count[MAX_COUNT_DIGITS];
    char buffer[INPUT_CHUNK_SIZE];
};
typedef struct input_buffer_s input_buffer;

struct output_buffer_s {
    FILE *hash_file;
    FILE *count_file;
    uint64_t hash_cursor;
    uint64_t count_cursor;
    char hash[OUTPUT_HASH_BUFFER_LENGTH];
    uint32_t count[OUTPUT_COUNT_BUFFER_LENGTH];
};
typedef struct output_buffer_s output_buffer;

inline void split_and_align_characters(input_buffer *input, uint64_t i, uint64_t max_size);

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Usage: %s <INPUT_FILE> <SHA1_OUT_FILE> <COUNT_OUT_FILE>\n", argv[0]);
        return 1;
    }

    char *input_filename = argv[1];
    char *sha1_output_filename = argv[2];
    char *count_output_filename = argv[3];

    struct timeval st, et;
    gettimeofday(&st, NULL);

    input_buffer input;
    input.file = fopen(input_filename, "r");
    input.buffer_cursor = 0;
    input.hash_cursor = 0;
    input.count_cursor = 0;
    input.is_reading_hash = true;
    input.hash[SHA1_UNCOMPRESSED_SIZE] = '\0';

    output_buffer output;
    output.hash_file = fopen(sha1_output_filename, "wb");
    output.count_file = fopen(count_output_filename, "wb");
    output.hash_cursor = 0;
    output.count_cursor = 0;

    fseek(input.file, 0L, SEEK_END);
    uint64_t input_file_length = ftell(input.file);
    fseek(input.file, 0L, SEEK_SET);

    for (uint64_t chunk_start = 0; chunk_start <= input_file_length; chunk_start += INPUT_CHUNK_SIZE) {
        uint64_t chunk_size = (chunk_start + INPUT_CHUNK_SIZE > input_file_length)
            ? (input_file_length - chunk_start)
            : (INPUT_CHUNK_SIZE);
        fread(&input.buffer, sizeof(char), chunk_size, input.file);
        for (uint64_t i = 0; i < chunk_size; i++) {
            split_and_align_characters(&input, i, chunk_size);
            if (input.buffer[i] == ':') {
                sha1_compress(&output.hash[output.hash_cursor], &input.hash[0]);
                output.hash_cursor += SHA1_COMPRESSED_SIZE;
                if (output.hash_cursor == OUTPUT_HASH_BUFFER_LENGTH) {
                    fwrite(&output.hash[0], sizeof(char), output.hash_cursor, output.hash_file);
                    output.hash_cursor = 0;
                }
            } 
            
            if (input.buffer[i] == '\n') {
                output.count[output.count_cursor] = atoi(&input.count[0]);
                output.count_cursor++;
                if (output.count_cursor == OUTPUT_COUNT_BUFFER_LENGTH) {
                    fwrite(&output.count[0], sizeof(uint32_t), output.count_cursor, output.count_file);
                    output.count_cursor = 0;
                }
            }
        }
    }

    if (output.hash_cursor > 0) {
        fwrite(&output.hash[0], sizeof(char), output.hash_cursor, output.hash_file);
        output.hash_cursor = 0;
    }

    if (output.count_cursor > 0) {
        fwrite(&output.count[0], sizeof(uint32_t), output.count_cursor, output.count_file);
        output.count_cursor = 0;
    }

    fclose(input.file);
    fclose(output.hash_file);
    fclose(output.count_file);
    gettimeofday(&et, NULL);
    long elapsed_micro_seconds = 1000000 * (et.tv_sec - st.tv_sec)
        + (et.tv_usec - st.tv_usec);
    setlocale(LC_NUMERIC, "en_US.utf-8");
    printf("Elapsed: %'8ld μs\n", elapsed_micro_seconds);
}

inline void split_and_align_characters(input_buffer *input, uint64_t i, uint64_t buffer_size) {
    if (input->buffer[i] == '\n') {
        memcpy(&input->count[input->count_cursor],
               &input->buffer[input->buffer_cursor],
               i - input->buffer_cursor);
        input->count[input->count_cursor + i - input->buffer_cursor - 1] = '\0';
        input->count_cursor = 0;
        input->buffer_cursor = i + 1;
        input->is_reading_hash = true;
    } else if (input->buffer[i] == ':') {
        memcpy(&input->hash[input->hash_cursor],
               &input->buffer[input->buffer_cursor],
               i - input->buffer_cursor);
        input->hash_cursor = 0;
        input->buffer_cursor = i + 1;
        input->is_reading_hash = false;
    }
    if (i == buffer_size - 1) {
        if (input->is_reading_hash) {
            memcpy(&input->hash[input->hash_cursor], 
                   &input->buffer[input->buffer_cursor], 
                   buffer_size - input->buffer_cursor);
            input->hash_cursor += buffer_size - input->buffer_cursor;
        } else {
            memcpy(&input->count[input->count_cursor],
                   &input->buffer[input->buffer_cursor],
                   buffer_size - input->buffer_cursor);
            input->count_cursor = buffer_size - input->buffer_cursor;
        }
        input->buffer_cursor = 0;
    }
}

