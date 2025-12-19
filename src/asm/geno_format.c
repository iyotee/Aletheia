/*
 * ALETHEIA Stage 1.3: GENO Object Format Implementation
 *
 * Implementation of the GENO object format and linker functionality.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "geno_format.h"

/* Utility functions */
static void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return ptr;
}

static void safe_free(void* ptr) {
    if (ptr) free(ptr);
}

/* GENO Object Loading */
GENO_Object* geno_load_object(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Failed to open GENO object file");
        return NULL;
    }

    GENO_Object* obj = safe_malloc(sizeof(GENO_Object));

    /* Read header */
    if (fread(&obj->header, sizeof(GENO_Header), 1, f) != 1) {
        fprintf(stderr, "Failed to read GENO header\n");
        free(obj);
        fclose(f);
        return NULL;
    }

    /* Verify magic */
    if (memcmp(obj->header.magic, GENO_MAGIC, 4) != 0) {
        fprintf(stderr, "Invalid GENO magic\n");
        free(obj);
        fclose(f);
        return NULL;
    }

    /* Verify version and architecture */
    if (obj->header.version != GENO_VERSION) {
        fprintf(stderr, "Unsupported GENO version\n");
        free(obj);
        fclose(f);
        return NULL;
    }

    if (obj->header.architecture != GENO_ARCH_X86_64) {
        fprintf(stderr, "Unsupported GENO architecture\n");
        free(obj);
        fclose(f);
        return NULL;
    }

    /* Allocate and read symbol table */
    if (obj->header.symbol_count > 0) {
        obj->symbols = safe_malloc(obj->header.symbol_count * sizeof(GENO_Symbol));
        if (fread(obj->symbols, sizeof(GENO_Symbol), obj->header.symbol_count, f) != obj->header.symbol_count) {
            fprintf(stderr, "Failed to read symbol table\n");
            geno_free_object(obj);
            fclose(f);
            return NULL;
        }
    } else {
        obj->symbols = NULL;
    }

    /* Allocate and read relocation table */
    if (obj->header.reloc_count > 0) {
        obj->relocations = safe_malloc(obj->header.reloc_count * sizeof(GENO_Relocation));
        if (fread(obj->relocations, sizeof(GENO_Relocation), obj->header.reloc_count, f) != obj->header.reloc_count) {
            fprintf(stderr, "Failed to read relocation table\n");
            geno_free_object(obj);
            fclose(f);
            return NULL;
        }
    } else {
        obj->relocations = NULL;
    }

    /* Allocate and read code section */
    if (obj->header.code_size > 0) {
        obj->code_section = safe_malloc(obj->header.code_size);
        if (fread(obj->code_section, 1, obj->header.code_size, f) != obj->header.code_size) {
            fprintf(stderr, "Failed to read code section\n");
            geno_free_object(obj);
            fclose(f);
            return NULL;
        }
    } else {
        obj->code_section = NULL;
    }

    /* Allocate and read data section */
    if (obj->header.data_size > 0) {
        obj->data_section = safe_malloc(obj->header.data_size);
        if (fread(obj->data_section, 1, obj->header.data_size, f) != obj->header.data_size) {
            fprintf(stderr, "Failed to read data section\n");
            geno_free_object(obj);
            fclose(f);
            return NULL;
        }
    } else {
        obj->data_section = NULL;
    }

    /* Allocate and read string table */
    if (obj->header.string_size > 0) {
        obj->string_table = safe_malloc(obj->header.string_size);
        if (fread(obj->string_table, 1, obj->header.string_size, f) != obj->header.string_size) {
            fprintf(stderr, "Failed to read string table\n");
            geno_free_object(obj);
            fclose(f);
            return NULL;
        }
    } else {
        obj->string_table = NULL;
    }

    /* Initialize computed fields */
    obj->code_base_address = 0;
    obj->data_base_address = 0;

    fclose(f);
    return obj;
}

void geno_free_object(GENO_Object* obj) {
    if (!obj) return;

    safe_free(obj->symbols);
    safe_free(obj->relocations);
    safe_free(obj->code_section);
    safe_free(obj->data_section);
    safe_free(obj->string_table);
    free(obj);
}

void geno_dump_object(GENO_Object* obj) {
    if (!obj) return;

    printf("GENO Object Dump:\n");
    printf("  Magic: %.4s\n", obj->header.magic);
    printf("  Version: %u\n", obj->header.version);
    printf("  Architecture: %u\n", obj->header.architecture);
    printf("  Symbols: %u\n", obj->header.symbol_count);
    printf("  Relocations: %u\n", obj->header.reloc_count);
    printf("  Code Size: %u bytes\n", obj->header.code_size);
    printf("  Data Size: %u bytes\n", obj->header.data_size);
    printf("  String Table: %u bytes\n", obj->header.string_size);

    /* Dump symbols */
    printf("  Symbol Table:\n");
    for (uint32_t i = 0; i < obj->header.symbol_count; i++) {
        const char* name = obj->string_table + obj->symbols[i].name_offset;
        printf("    [%u] %s (type=%u, addr=0x%x, size=%u)\n",
               i, name, obj->symbols[i].type, obj->symbols[i].address, obj->symbols[i].size);
    }

    /* Dump relocations */
    printf("  Relocation Table:\n");
    for (uint32_t i = 0; i < obj->header.reloc_count; i++) {
        printf("    [%u] offset=0x%x, type=%u, symbol=%u\n",
               i, obj->relocations[i].offset, obj->relocations[i].type, obj->relocations[i].symbol_index);
    }
}

/* Linker Implementation */
LinkerContext* linker_create_context() {
    LinkerContext* ctx = safe_malloc(sizeof(LinkerContext));
    ctx->objects = NULL;
    ctx->object_count = 0;
    ctx->global_symbols = NULL;
    ctx->symbol_count = 0;
    ctx->symbol_capacity = 0;
    ctx->output_code = NULL;
    ctx->code_size = 0;
    ctx->output_data = NULL;
    ctx->data_size = 0;
    ctx->code_base = 0x1000;  /* Code starts at 0x1000 */
    ctx->data_base = 0x2000;  /* Data starts at 0x2000 */
    return ctx;
}

void linker_free_context(LinkerContext* ctx) {
    if (!ctx) return;

    for (int i = 0; i < ctx->object_count; i++) {
        geno_free_object(ctx->objects[i]);
    }
    safe_free(ctx->objects);
    safe_free(ctx->global_symbols);
    safe_free(ctx->output_code);
    safe_free(ctx->output_data);
    free(ctx);
}

void linker_add_object(LinkerContext* ctx, GENO_Object* obj) {
    /* Add object to context */
    ctx->objects = realloc(ctx->objects, (ctx->object_count + 1) * sizeof(GENO_Object*));
    ctx->objects[ctx->object_count++] = obj;

    /* Set base addresses for this object */
    obj->code_base_address = ctx->code_base + ctx->code_size;
    obj->data_base_address = ctx->data_base + ctx->data_size;

    /* Expand output sections */
    ctx->output_code = realloc(ctx->output_code, ctx->code_size + obj->header.code_size);
    ctx->output_data = realloc(ctx->output_data, ctx->data_size + obj->header.data_size);

    /* Copy sections */
    if (obj->header.code_size > 0) {
        memcpy(ctx->output_code + ctx->code_size, obj->code_section, obj->header.code_size);
    }
    if (obj->header.data_size > 0) {
        memcpy(ctx->output_data + ctx->data_size, obj->data_section, obj->header.data_size);
    }

    /* Update sizes */
    ctx->code_size += obj->header.code_size;
    ctx->data_size += obj->header.data_size;

    /* Collect symbols */
    for (uint32_t i = 0; i < obj->header.symbol_count; i++) {
        GENO_Symbol* sym = &obj->symbols[i];

        /* Expand symbol table if needed */
        if (ctx->symbol_count >= ctx->symbol_capacity) {
            ctx->symbol_capacity = ctx->symbol_capacity ? ctx->symbol_capacity * 2 : 16;
            ctx->global_symbols = realloc(ctx->global_symbols, ctx->symbol_capacity * sizeof(LinkerSymbol));
        }

        /* Add symbol */
        LinkerSymbol* link_sym = &ctx->global_symbols[ctx->symbol_count++];
        link_sym->name = obj->string_table + sym->name_offset;
        link_sym->type = sym->type;
        link_sym->size = sym->size;
        link_sym->defined = 1;
        link_sym->source_object = obj;

        /* Calculate absolute address */
        if (sym->type == GENO_SYM_FUNCTION || sym->type == GENO_SYM_GLOBAL_VAR) {
            link_sym->address = obj->code_base_address + sym->address;
        } else {
            link_sym->address = obj->data_base_address + sym->address;
        }
    }
}

int linker_resolve_symbols(LinkerContext* ctx) {
    /* For now, assume all symbols are resolved within the objects */
    /* Future: handle external symbols and libraries */
    printf("Symbol resolution: %d symbols processed\n", ctx->symbol_count);
    return 1;  /* Success */
}

int linker_apply_relocations(LinkerContext* ctx) {
    int reloc_count = 0;

    for (int obj_idx = 0; obj_idx < ctx->object_count; obj_idx++) {
        GENO_Object* obj = ctx->objects[obj_idx];

        for (uint32_t reloc_idx = 0; reloc_idx < obj->header.reloc_count; reloc_idx++) {
            GENO_Relocation* reloc = &obj->relocations[reloc_idx];

            if (reloc->symbol_index >= ctx->symbol_count) {
                fprintf(stderr, "Invalid symbol index in relocation\n");
                return 0;
            }

            LinkerSymbol* sym = &ctx->global_symbols[reloc->symbol_index];
            uint32_t target_address = sym->address;

            /* Apply relocation based on type */
            if (reloc->type == GENO_REL_ABSOLUTE) {
                /* Absolute 64-bit address */
                uint32_t* patch_location;
                if (reloc->offset < obj->header.code_size) {
                    patch_location = (uint32_t*)(ctx->output_code + (obj->code_base_address - ctx->code_base) + reloc->offset);
                } else {
                    patch_location = (uint32_t*)(ctx->output_data + (obj->data_base_address - ctx->data_base) + (reloc->offset - obj->header.code_size));
                }
                *patch_location = target_address;

            } else if (reloc->type == GENO_REL_RELATIVE) {
                /* Relative 32-bit offset */
                uint32_t* patch_location;
                uint32_t current_addr;

                if (reloc->offset < obj->header.code_size) {
                    patch_location = (uint32_t*)(ctx->output_code + (obj->code_base_address - ctx->code_base) + reloc->offset);
                    current_addr = obj->code_base_address + reloc->offset + 4;  /* +4 for the offset itself */
                } else {
                    patch_location = (uint32_t*)(ctx->output_data + (obj->data_base_address - ctx->data_base) + (reloc->offset - obj->header.code_size));
                    current_addr = obj->data_base_address + reloc->offset + 4;
                }

                int32_t offset = (int32_t)(target_address - current_addr);
                *patch_location = (uint32_t)offset;
            }

            reloc_count++;
        }
    }

    printf("Applied %d relocations\n", reloc_count);
    return 1;  /* Success */
}

void linker_generate_executable(LinkerContext* ctx, const char* output_file) {
    FILE* f = fopen(output_file, "wb");
    if (!f) {
        perror("Failed to create executable");
        return;
    }

    /* Calculate total size */
    uint64_t code_start = 0x78;  /* After ELF headers */
    uint64_t data_start = code_start + ctx->code_size;
    uint64_t total_size = data_start + ctx->data_size;

    /* ELF Header (64 bytes) */
    uint8_t elf_header[64] = {
        0x7F, 'E', 'L', 'F',  // Magic
        2, 1, 1, 0,          // 64-bit, little-endian, ELF v1
        0, 0, 0, 0, 0, 0, 0, 0,  // Padding
        2, 0,                 // ET_EXEC
        0x3E, 0,              // EM_X86_64
        1, 0, 0, 0,           // EV_CURRENT
        0, 0, 0, 0, 0, 0, 0, 0,  // Entry point (will be set)
        64, 0, 0, 0, 0, 0, 0, 0, // Program header offset
        0, 0, 0, 0, 0, 0, 0, 0,  // Section header offset
        0, 0, 0, 0,            // Flags
        64, 0,                 // ELF header size
        56, 0,                 // Program header size
        1, 0,                  // Number of program headers
        0, 0,                  // Section header size
        0, 0,                  // Number of section headers
        0, 0                   // Section header string table index
    };

    /* Set entry point to start of code */
    memcpy(&elf_header[24], &code_start, 8);

    /* Program Header (56 bytes) */
    uint8_t program_header[56] = {
        1, 0, 0, 0,          // PT_LOAD
        7, 0, 0, 0,          // PF_R | PF_W | PF_X (read + write + execute)
        0, 0, 0, 0, 0, 0, 0, 0,  // File offset
        0, 0, 0, 0, 0, 0, 0, 0,  // Virtual address
        0, 0, 0, 0, 0, 0, 0, 0,  // Physical address
        0, 0, 0, 0, 0, 0, 0, 0,  // File size (will be set)
        0, 0, 0, 0, 0, 0, 0, 0,  // Memory size (will be set)
        0, 0, 0, 0, 0, 0, 0, 0   // Alignment
    };

    /* Set file and memory sizes */
    memcpy(&program_header[32], &total_size, 8);  // File size
    memcpy(&program_header[40], &total_size, 8);  // Memory size

    /* Set alignment */
    uint64_t alignment = 0x1000;
    memcpy(&program_header[48], &alignment, 8);

    /* Write ELF header */
    fwrite(elf_header, 1, 64, f);

    /* Write program header */
    fwrite(program_header, 1, 56, f);

    /* Write code section */
    if (ctx->code_size > 0) {
        fwrite(ctx->output_code, 1, ctx->code_size, f);
    }

    /* Write data section */
    if (ctx->data_size > 0) {
        fwrite(ctx->output_data, 1, ctx->data_size, f);
    }

    fclose(f);
    printf("Generated executable: %s (%lu bytes)\n", output_file, total_size);
}
