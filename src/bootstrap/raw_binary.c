/*
 * ALETHEIA Stage 0: Raw Binary Emission
 *
 * This program manually emits x86-64 machine code opcodes
 * to create a minimal executable that returns exit code 42.
 *
 * NO assembler, NO labels, NO symbols - only raw opcodes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * x86-64 opcode encoding functions
 * All encoding is done manually according to Intel/AMD specifications
 */

void emit_byte(FILE* f, uint8_t byte) {
    fwrite(&byte, 1, 1, f);
}

void emit_word(FILE* f, uint16_t word) {
    emit_byte(f, word & 0xFF);
    emit_byte(f, (word >> 8) & 0xFF);
}

void emit_dword(FILE* f, uint32_t dword) {
    emit_word(f, dword & 0xFFFF);
    emit_word(f, (dword >> 16) & 0xFFFF);
}

void emit_qword(FILE* f, uint64_t qword) {
    emit_dword(f, qword & 0xFFFFFFFF);
    emit_dword(f, (qword >> 32) & 0xFFFFFFFF);
}

/*
 * ELF64 header emission
 * Manual encoding according to ELF specification
 */
void emit_elf_header(FILE* f) {
    // ELF magic and identification
    emit_byte(f, 0x7F); emit_byte(f, 'E'); emit_byte(f, 'L'); emit_byte(f, 'F'); // ELF magic
    emit_byte(f, 2);    // 64-bit
    emit_byte(f, 1);    // Little-endian
    emit_byte(f, 1);    // ELF version
    emit_byte(f, 0);    // ABI
    emit_byte(f, 0);    // ABI version
    emit_byte(f, 0); emit_byte(f, 0); emit_byte(f, 0); emit_byte(f, 0); // Padding
    emit_byte(f, 0); emit_byte(f, 0); emit_byte(f, 0); emit_byte(f, 0);

    // ELF header fields
    emit_word(f, 2);    // ET_EXEC (executable)
    emit_word(f, 62);   // EM_X86_64
    emit_dword(f, 1);   // EV_CURRENT

    // Entry point (0x78)
    emit_qword(f, 0x78);

    // Program header offset (0x40)
    emit_qword(f, 0x40);

    // Section header offset (0)
    emit_qword(f, 0);

    // Flags (0)
    emit_dword(f, 0);

    // Header sizes
    emit_word(f, 64);   // ELF header size
    emit_word(f, 56);   // Program header size
    emit_word(f, 1);    // Number of program headers
    emit_word(f, 0);    // Section header size
    emit_word(f, 0);    // Number of section headers
    emit_word(f, 0);    // Section header string table index
}

/*
 * Program header emission
 * Defines the loadable segment
 */
void emit_program_header(FILE* f) {
    emit_dword(f, 1);   // PT_LOAD
    emit_dword(f, 5);   // PF_R | PF_X (read + execute)

    // Offsets and addresses
    emit_qword(f, 0);   // File offset
    emit_qword(f, 0);   // Virtual address
    emit_qword(f, 0);   // Physical address

    // Sizes (150 bytes total: 64 + 56 + 30)
    emit_qword(f, 150); // File size
    emit_qword(f, 150); // Memory size

    // Alignment (4096 bytes)
    emit_qword(f, 0x1000);
}

/*
 * Code section emission
 * Manually encoded x86-64 instructions for:
 * mov rax, 60   (exit syscall number)
 * mov rdi, 42   (exit code)
 * syscall       (make system call)
 */
void emit_code_section(FILE* f) {
    // mov rax, 60 (exit syscall)
    // REX.W prefix (48) + opcode C7 C0 + immediate
    emit_byte(f, 0x48); // REX.W
    emit_byte(f, 0xC7); // MOV immediate to register
    emit_byte(f, 0xC0); // RAX register
    emit_dword(f, 60);  // Syscall number for exit

    // mov rdi, 42 (exit code)
    // REX.W prefix (48) + opcode BF + immediate
    emit_byte(f, 0x48); // REX.W
    emit_byte(f, 0xBF); // MOV immediate to RDI
    emit_qword(f, 42);  // Exit code

    // syscall
    emit_byte(f, 0x0F); // Two-byte opcode prefix
    emit_byte(f, 0x05); // SYSCALL opcode
}

int main() {
    FILE* f = fopen("build/bootstrap.bin", "wb");
    if (!f) {
        perror("Failed to create bootstrap.bin");
        return 1;
    }

    printf("ALETHEIA Stage 0: Emitting raw x86-64 binary...\n");

    long bytes_written = 0;

    // Emit ELF64 header (64 bytes)
    long header_start = ftell(f);
    emit_elf_header(f);
    long header_end = ftell(f);
    printf("✓ ELF header emitted (%ld bytes)\n", header_end - header_start);
    bytes_written += header_end - header_start;

    // Emit program header (56 bytes)
    long phdr_start = ftell(f);
    emit_program_header(f);
    long phdr_end = ftell(f);
    printf("✓ Program header emitted (%ld bytes)\n", phdr_end - phdr_start);
    bytes_written += phdr_end - phdr_start;

    // Emit code section (30 bytes)
    long code_start = ftell(f);
    emit_code_section(f);
    long code_end = ftell(f);
    printf("✓ Code section emitted (%ld bytes)\n", code_end - code_start);
    bytes_written += code_end - code_start;

    fclose(f);

    printf("✓ Raw binary emission complete (%ld bytes total)\n", bytes_written);
    printf("Binary saved as: build/bootstrap.bin\n");

    // Verification
    printf("\nVerifying binary...\n");
    FILE* verify = fopen("build/bootstrap.bin", "rb");
    if (verify) {
        fseek(verify, 0, SEEK_END);
        long size = ftell(verify);
        fclose(verify);
        printf("✓ Binary size: %ld bytes (expected: 150)\n", size);

        if (size == 150) {
            printf("✓ Binary size verification PASSED\n");
        } else {
            printf("✗ Binary size verification FAILED\n");
            return 1;
        }
    }

    printf("\n🎉 ALETHEIA Stage 0 complete!\n");
    printf("This binary represents the absolute foundation of trust.\n");
    printf("All future ALETHEIA components will be built from this.\n");

    return 0;
}
