// ALETHEIA Multi-Target Backend Interface
// Unified interface for all target architectures (x86-64, ARM64, RISC-V)

#ifndef ALETHEIA_BACKEND_H
#define ALETHEIA_BACKEND_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Target architecture enumeration
typedef enum {
    TARGET_X86_64,
    TARGET_ARM64,
    TARGET_RISCV64
} TargetArch;

// Register classes
typedef enum {
    REG_CLASS_GP,  // General purpose
    REG_CLASS_VEC, // Vector/SIMD
    REG_CLASS_SP   // Special purpose (SP, PC, etc.)
} RegisterClass;

// Unified register representation
typedef struct {
    const char* name;
    RegisterClass class;
    int number;
    bool preserved; // Callee-saved
} TargetRegister;

// Calling convention information
typedef struct {
    TargetRegister** arg_registers;  // Registers for arguments
    int num_arg_registers;
    TargetRegister* return_register; // Return value register
    TargetRegister* stack_pointer;   // Stack pointer
    TargetRegister* frame_pointer;   // Frame pointer (optional)
    int stack_alignment;            // Stack alignment requirement
    bool caller_cleanup;           // Who cleans up stack
} CallingConvention;

// Instruction information
typedef struct {
    const char* mnemonic;
    int max_operands;
    bool supports_immediate;
} TargetInstruction;

// Backend interface
typedef struct {
    TargetArch arch;
    const char* name;
    const char* triple; // LLVM-style triple (e.g., "aarch64-linux-gnu")

    // Registers
    TargetRegister** registers;
    int num_registers;

    // Calling convention
    CallingConvention* calling_convention;

    // Instructions
    TargetInstruction** instructions;
    int num_instructions;

    // Code generation functions
    void (*generate_prologue)(FILE* out, int stack_size);
    void (*generate_epilogue)(FILE* out, int stack_size);
    void (*generate_mov)(FILE* out, const char* dest, const char* src);
    void (*generate_add)(FILE* out, const char* dest, const char* src1, const char* src2);
    void (*generate_sub)(FILE* out, const char* dest, const char* src1, const char* src2);
    void (*generate_mul)(FILE* out, const char* dest, const char* src1, const char* src2);
    void (*generate_div)(FILE* out, const char* dest, const char* src1, const char* src2);
    void (*generate_load)(FILE* out, const char* dest, const char* addr, int offset);
    void (*generate_store)(FILE* out, const char* src, const char* addr, int offset);
    void (*generate_cmp)(FILE* out, const char* op1, const char* op2);
    void (*generate_jmp)(FILE* out, const char* label);
    void (*generate_je)(FILE* out, const char* label);
    void (*generate_jne)(FILE* out, const char* label);
    void (*generate_jl)(FILE* out, const char* label);
    void (*generate_jg)(FILE* out, const char* label);
    void (*generate_call)(FILE* out, const char* function);
    void (*generate_ret)(FILE* out);
    void (*generate_label)(FILE* out, const char* label);

    // IA integration
    void (*apply_ia_hints)(FILE* out, const char* optimization_type);

} TargetBackend;

// Global backend registry
extern TargetBackend* current_backend;

// Backend management functions
TargetBackend* create_x86_64_backend(void);
TargetBackend* create_arm64_backend(void);
TargetBackend* create_riscv64_backend(void);

void set_current_backend(TargetArch arch);
TargetBackend* get_current_backend(void);

// Code generation helpers
void emit_instruction(FILE* out, const char* format, ...);
void emit_label(FILE* out, const char* label);
void emit_comment(FILE* out, const char* comment);

// Architecture detection and setup
TargetArch detect_host_architecture(void);
const char* get_architecture_name(TargetArch arch);
const char* get_architecture_triple(TargetArch arch);

// IA-aware code generation
void generate_ia_optimized_code(TargetBackend* backend, FILE* out,
                               const char* optimization_type,
                               const char* code_pattern);

#endif // ALETHEIA_BACKEND_H
