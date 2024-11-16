#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

#define BYTE_RANGE 256

typedef struct Node {
    unsigned char byte;
    unsigned int freq;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    unsigned char bits[BYTE_RANGE];
    unsigned int size;
} Code;

static unsigned int frequencies[BYTE_RANGE];
static Code codes[BYTE_RANGE];

static void calculate_frequencies(FILE *file) {
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        frequencies[ch]++;
    }
}

static Node *create_node(unsigned char byte, unsigned int freq, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->byte = byte;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

static int compare_nodes(const void *a, const void *b) {
    Node *node_a = *(Node **)a;
    Node *node_b = *(Node **)b;
    return node_a->freq - node_b->freq;
}

static Node *build_huffman_tree() {
    Node *nodes[BYTE_RANGE];
    int count = 0;

    for (int i = 0; i < BYTE_RANGE; i++) {
        if (frequencies[i] > 0) {
            nodes[count++] = create_node((unsigned char)i, frequencies[i], NULL, NULL);
        }
    }

    while (count > 1) {
        qsort(nodes, count, sizeof(Node *), compare_nodes);
        Node *left = nodes[0];
        Node *right = nodes[1];

        Node *parent = create_node(0, left->freq + right->freq, left, right);

        nodes[0] = parent;
        memmove(&nodes[1], &nodes[2], (count - 2) * sizeof(Node *));
        count--;
    }

    return count == 1 ? nodes[0] : NULL;
}

static void generate_codes(Node *root, unsigned char *code, int depth) {
    if (root->left == NULL && root->right == NULL) {
        codes[root->byte].size = depth;
        memcpy(codes[root->byte].bits, code, depth);
        return;
    }
    if (root->left) {
        code[depth] = 0;
        generate_codes(root->left, code, depth + 1);
    }
    if (root->right) {
        code[depth] = 1;
        generate_codes(root->right, code, depth + 1);
    }
}

static void free_huffman_tree(Node *root) {
    if (root) {
        free_huffman_tree(root->left);
        free_huffman_tree(root->right);
        free(root);
    }
}

int compress_file(const char *input_path, const char *output_path) {
    FILE *input = fopen(input_path, "rb");
    FILE *output = fopen(output_path, "wb");
    if (!input || !output) {
        perror("File opening failed");
        if (input) fclose(input);
        if (output) fclose(output);
        return -1;
    }

    memset(frequencies, 0, sizeof(frequencies));

    calculate_frequencies(input);
    rewind(input);

    unsigned int total_symbols = 0;
    for (int i = 0; i < BYTE_RANGE; i++) {
        total_symbols += frequencies[i];
    }

    Node *huffman_tree = build_huffman_tree();
    if (!huffman_tree) {
        fprintf(stderr, "Failed to build Huffman tree.\n");
        fclose(input);
        fclose(output);
        return -1;
    }

    unsigned char code_buffer[BYTE_RANGE];
    generate_codes(huffman_tree, code_buffer, 0);

    if (fwrite(&total_symbols, sizeof(total_symbols), 1, output) != 1) {
        fprintf(stderr, "Failed to write total_symbols to output file.\n");
        free_huffman_tree(huffman_tree);
        fclose(input);
        fclose(output);
        return -1;
    }
    if (fwrite(frequencies, sizeof(frequencies), 1, output) != 1) {
        fprintf(stderr, "Failed to write frequencies to output file.\n");
        free_huffman_tree(huffman_tree);
        fclose(input);
        fclose(output);
        return -1;
    }

    unsigned char buffer = 0;
    int bit_count = 0;
    int ch;
    while ((ch = fgetc(input)) != EOF) {
        Code code = codes[ch];
        for (unsigned int i = 0; i < code.size; i++) {
            buffer = (buffer << 1) | code.bits[i];
            bit_count++;
            if (bit_count == 8) {
                fputc(buffer, output);
                buffer = 0;
                bit_count = 0;
            }
        }
    }
    if (bit_count > 0) {
        buffer <<= (8 - bit_count);
        fputc(buffer, output);
    }

    free_huffman_tree(huffman_tree);
    fclose(input);
    fclose(output);
    return 0;
}

int decompress_file(const char *input_path, const char *output_path) {
    FILE *input = fopen(input_path, "rb");
    FILE *output = fopen(output_path, "wb");
    if (!input || !output) {
        perror("File opening failed");
        if (input) fclose(input);
        if (output) fclose(output);
        return -1;
    }

    unsigned int total_symbols;
    if (fread(&total_symbols, sizeof(total_symbols), 1, input) != 1) {
        fprintf(stderr, "Failed to read total_symbols from input file.\n");
        fclose(input);
        fclose(output);
        return -1;
    }
    if (fread(frequencies, sizeof(frequencies), 1, input) != 1) {
        fprintf(stderr, "Failed to read frequencies from input file.\n");
        fclose(input);
        fclose(output);
        return -1;
    }

    Node *huffman_tree = build_huffman_tree();
    if (!huffman_tree) {
        fprintf(stderr, "Failed to rebuild Huffman tree.\n");
        fclose(input);
        fclose(output);
        return -1;
    }

    Node *current = huffman_tree;
    int ch;
    unsigned char byte;
    unsigned int symbols_decoded = 0;

    while (symbols_decoded < total_symbols && (ch = fgetc(input)) != EOF) {
        byte = (unsigned char)ch;
        for (int i = 7; i >= 0; i--) {
            int bit = (byte >> i) & 1;
            if (bit == 0)
                current = current->left;
            else
                current = current->right;

            if (current->left == NULL && current->right == NULL) {
                fputc(current->byte, output);
                current = huffman_tree;
                symbols_decoded++;
                if (symbols_decoded == total_symbols)
                    break;
            }
        }
    }

    free_huffman_tree(huffman_tree);
    fclose(input);
    fclose(output);
    return 0;
}
