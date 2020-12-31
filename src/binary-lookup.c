#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/time.h>
#include <ctype.h>
#include "sha1-compression.h"


struct results_s { int32_t seek_count; int64_t match_index; int32_t match_count; };
struct sha1_input { FILE* file; size_t max_index; char buffer[SHA1_COMPRESSED_SIZE]; };
struct count_input { FILE* file; };
void binary_file_search(struct results_s *results, struct sha1_input *sha1, char *hash);
size_t number_of_matches(char* hash, char* sha1_filename, char* count_filename);
void str_to_upper(char* str);

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Usage: %s <SHA1> <SHA1_FILE> <COUNT_FILE>", argv[0]);
        return 1;
    }
    char *hash = argv[1];
    char *sha1_filename = argv[2];
    char *count_filename = argv[3];

    struct timeval st, et;
    gettimeofday(&st, NULL);

    str_to_upper(hash);
    int32_t match_count = number_of_matches(hash, sha1_filename, count_filename);

    gettimeofday(&et, NULL);
    long elapsed_micro_seconds = 1000000 * (et.tv_sec - st.tv_sec)
        + (et.tv_usec - st.tv_usec);
        
    printf("Search time: %'8ld μs\n", elapsed_micro_seconds);
    printf("# of leaks:  %'8d\n", match_count);
}

size_t number_of_matches(char* hash, char* sha1_filename, char* count_filename) {
    struct count_input count;
    struct results_s results;
    struct sha1_input sha1;

    sha1.file = fopen(sha1_filename, "rb");
    fseek(sha1.file, 0, SEEK_END);
    sha1.max_index = ftell(sha1.file) / SHA1_COMPRESSED_SIZE;
    fseek(sha1.file, 0, SEEK_SET);
    binary_file_search(&results, &sha1, hash);
    fclose(sha1.file);

    count.file = fopen(count_filename, "rb");
    if (results.match_index == -1) {
        results.match_count = 0;
    } else {
        fseek(count.file, sizeof(int) * results.match_index, SEEK_SET);
        fread(&results.match_count, sizeof(int), 1, count.file);
    }
    fclose(count.file);
    return results.match_count;
}


void binary_file_search(struct results_s *results, struct sha1_input *sha1, char *hash) {
    char compressed_hash[SHA1_COMPRESSED_SIZE];
    sha1_compress(&compressed_hash[0], hash);
    results->seek_count = 0;
    results->match_index = -1;
    int low = 0;
    int high = sha1->max_index;
    size_t guess;
    int cmp;
    #ifdef DEBUG
    printf("+-----------+-----+------------------------------------------+\n");
    printf("|     index | cmp | hash                                     |\n");
    printf("+-----------+-----+------------------------------------------+\n");
    #endif
    while ((high + low) / 2 != guess) {
        results->seek_count++;
        guess = (low + high) / 2;
        fseek(sha1->file, SHA1_COMPRESSED_SIZE * guess, SEEK_SET);
        fread(&sha1->buffer, sizeof(char), SHA1_COMPRESSED_SIZE, sha1->file);
        cmp = sha1_compare_compressed(&compressed_hash[0], &sha1->buffer[0]);

        #ifdef DEBUG
        char tmp1[41]; tmp1[40] = '\0';
        char tmp2[41]; tmp2[40] = '\0';
        sha1_decompress(&tmp1[0], &sha1->buffer[0]);
        sha1_decompress(&tmp2[0], &compressed_hash[0]);
        printf("| %9d | %3d | %40s |\n", guess, cmp, tmp1);
        #endif

        if (cmp > 0) { low = guess; continue; }
        if (cmp < 0) { high = guess; continue; }
        results->match_index = guess;
        break;
    }
    #ifdef DEBUG
    printf("+-----------+-----+------------------------------------------+\n");
    printf("Seeks:       %'8d\n", results->seek_count);
    #endif
}

void str_to_upper(char* str) {
    char *c = str;
    while(*c) {
        *c = toupper((unsigned char) *c);
        c++;
    }
}
