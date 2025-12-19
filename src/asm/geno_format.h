/*
 * ALETHEIA Stage 1.3: GENO Object Format
 *
 * GENO (Genesis Object) is ALETHEIA's custom object file format.
 * It provides symbol tables, relocations, and sections for linking.
 */

#ifndef GENO_FORMAT_H
#define GENO_FORMAT_H

#include <stdint.h>

/* GENO Magic Number */
#define GENO_MAGIC "GENO"
#define GENO_VERSION 1

/* Architecture Codes */
#define GENO_ARCH_X86_64 1

/* Symbol Types */
#define GENO_SYM_UNDEFINED 0
#define GENO_SYM_FUNCTION  1
#define GENO_SYM_GLOBAL_VAR 2
#define GENO_SYM_LOCAL_VAR  3

/* Relocation Types */
#define GENO_REL_ABSOLUTE 1  /* Absolute 64-bit address */
#define GENO_REL_RELATIVE 2  /* Relative 32-bit offset */
#define GENO_REL_PC_REL   3  /* PC-relative 32-bit offset */

/* GENO Header (64 bytes) */
typedef struct {
    char magic[4];          /* "GENO" */
    uint32_t version;       /* Format version */
    uint32_t architecture;  /* Target architecture */
    uint32_t symbol_count;  /* Number of symbols */
    uint32_t reloc_count;   /* Number of relocations */
    uint32_t code_size;     /* Code section size */
    uint32_t data_size;     /* Data section size */
    uint32_t string_size;   /* String table size */
    uint32_t reserved[4];   /* Reserved for future use */
} GENO_Header;

/* GENO Symbol (16 bytes) */
typedef struct {
    uint32_t name_offset;   /* Offset in string table */
    uint32_t type;          /* Symbol type */
    uint32_t address;       /* Address in section */
    uint32_t size;          /* Size of symbol */
} GENO_Symbol;

/* GENO Relocation (12 bytes) */
typedef struct {
    uint32_t offset;        /* Offset in section to patch */
    uint32_t type;          /* Relocation type */
    uint32_t symbol_index;  /* Index in symbol table */
} GENO_Relocation;

/* Complete GENO File Layout:
 *
 * +-------------------+
 * | GENO_Header      |  64 bytes
 * +-------------------+
 * | Symbol Table     |  symbol_count * 16 bytes
 * +-------------------+
 * | Relocation Table |  reloc_count * 12 bytes
 * +-------------------+
 * | Code Section     |  code_size bytes
 * +-------------------+
 * | Data Section     |  data_size bytes
 * +-------------------+
 * | String Table     |  string_size bytes
 * +-------------------+
 */

/* In-memory representation for linker */
typedef struct {
    GENO_Header header;
    GENO_Symbol* symbols;
    GENO_Relocation* relocations;
    uint8_t* code_section;
    uint8_t* data_section;
    char* string_table;

    /* Computed fields */
    uint32_t code_base_address;
    uint32_t data_base_address;
} GENO_Object;

/* Linker symbol resolution */
typedef struct {
    char* name;
    uint32_t type;
    uint32_t address;
    uint32_t size;
    int defined;  /* 1 if defined, 0 if external */
    GENO_Object* source_object;
} LinkerSymbol;

/* Linker context */
typedef struct {
    GENO_Object** objects;
    int object_count;
    LinkerSymbol* global_symbols;
    int symbol_count;
    int symbol_capacity;

    /* Output sections */
    uint8_t* output_code;
    uint32_t code_size;
    uint8_t* output_data;
    uint32_t data_size;

    /* Base addresses for sections */
    uint32_t code_base;
    uint32_t data_base;
} LinkerContext;

/* API Functions */
GENO_Object* geno_load_object(const char* filename);
void geno_free_object(GENO_Object* obj);
void geno_dump_object(GENO_Object* obj);

LinkerContext* linker_create_context();
void linker_add_object(LinkerContext* ctx, GENO_Object* obj);
int linker_resolve_symbols(LinkerContext* ctx);
int linker_apply_relocations(LinkerContext* ctx);
void linker_generate_executable(LinkerContext* ctx, const char* output_file);
void linker_free_context(LinkerContext* ctx);

#endif /* GENO_FORMAT_H */
