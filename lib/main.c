#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

void print_usage(const char *program_name) {
    printf("Usage: %s [-c|-d] <input_file> <output_file>\n", program_name);
    printf("Options:\n");
    printf("  -c\tCompress the input file\n");
    printf("  -d\tDecompress the input file\n");
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-c") == 0) {
        if (compress_file(argv[2], argv[3]) != 0) {
            fprintf(stderr, "Compression failed.\n");
            return EXIT_FAILURE;
        }
        printf("File compressed successfully.\n");
    } else if (strcmp(argv[1], "-d") == 0) {
        if (decompress_file(argv[2], argv[3]) != 0) {
            fprintf(stderr, "Decompression failed.\n");
            return EXIT_FAILURE;
        }
        printf("File decompressed successfully.\n");
    } else {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
