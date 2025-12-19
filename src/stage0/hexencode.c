/*
 * Temporary hex encoder for bootstrap format
 * This will be removed once the pure hex loader works
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.c>\n", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");
    if (!f) {
        perror("Cannot open input file");
        return 1;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Write header
    printf("# ALETHEIA_BOOTSTRAP v1.0\n");
    printf("# File: %s\n", argv[1]);
    printf("# Size: %ld bytes\n", size);
    printf("# Checksum: 0000\n"); // Placeholder
    printf("# Begin data\n");

    // Read and encode file
    int count = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        printf("%02X", (unsigned char)c);
        count++;
        if (count % 16 == 0) {
            printf("\n");
        } else {
            printf(" ");
        }
    }

    printf("\n# End data\n");
    printf("# EOF\n");

    fclose(f);
    return 0;
}

