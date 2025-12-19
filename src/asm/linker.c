/*
 * ALETHEIA Stage 1.3: Linker
 *
 * Links multiple GENO object files into a single ELF64 executable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geno_format.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <output> <object1.geno> [object2.geno ...]\n", argv[0]);
        return 1;
    }

    const char* output_file = argv[1];
    int object_count = argc - 2;
    const char** object_files = (const char**)&argv[2];

    printf("ALETHEIA Stage 1.3: Linker\n");
    printf("Linking %d object files to: %s\n", object_count, output_file);

    /* Create linker context */
    LinkerContext* ctx = linker_create_context();
    if (!ctx) {
        fprintf(stderr, "Failed to create linker context\n");
        return 1;
    }

    /* Load all object files */
    for (int i = 0; i < object_count; i++) {
        printf("Loading object: %s\n", object_files[i]);
        GENO_Object* obj = geno_load_object(object_files[i]);
        if (!obj) {
            fprintf(stderr, "Failed to load object: %s\n", object_files[i]);
            linker_free_context(ctx);
            return 1;
        }

        /* Dump object for debugging */
        printf("  ");
        geno_dump_object(obj);

        /* Add to linker context */
        linker_add_object(ctx, obj);
    }

    printf("Loaded %d objects, total code: %u bytes, data: %u bytes\n",
           ctx->object_count, ctx->code_size, ctx->data_size);

    /* Resolve symbols */
    if (!linker_resolve_symbols(ctx)) {
        fprintf(stderr, "Symbol resolution failed\n");
        linker_free_context(ctx);
        return 1;
    }

    /* Apply relocations */
    if (!linker_apply_relocations(ctx)) {
        fprintf(stderr, "Relocation application failed\n");
        linker_free_context(ctx);
        return 1;
    }

    /* Generate executable */
    linker_generate_executable(ctx, output_file);

    /* Cleanup */
    linker_free_context(ctx);

    printf("Linking completed successfully!\n");
    return 0;
}
