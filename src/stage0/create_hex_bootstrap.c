#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return 1;
    }
    
    FILE* in = fopen(argv[1], "rb");
    FILE* out = fopen(argv[2], "w");
    if (!in || !out) return 1;
    
    fprintf(out, "# ALETHEIA_BOOTSTRAP v1.0\n");
    fprintf(out, "# Begin data\n");
    
    int ch, count = 0;
    while ((ch = fgetc(in)) != EOF) {
        fprintf(out, "%02X", (unsigned char)ch);
        count++;
        if (count % 32 == 0) fprintf(out, "\n");
        else if (count % 2 == 0) fprintf(out, " ");
    }
    
    fprintf(out, "\n# End data\n");
    fclose(in);
    fclose(out);
    
    printf("Created hex file: %s (%d bytes)\n", argv[2], count);
    return 0;
}