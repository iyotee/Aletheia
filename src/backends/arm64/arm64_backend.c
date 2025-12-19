// ALETHEIA ARM64 Backend Implementation
// AArch64 code generation for mobile and server platforms

#include "../backend.h"
#include <stdlib.h>
#include <string.h>

// ARM64 registers (64-bit)
static TargetRegister arm64_registers[] = {
    // General purpose registers
    {"x0", REG_CLASS_GP, 0, false},   // Argument/return register
    {"x1", REG_CLASS_GP, 1, false},   // Argument register
    {"x2", REG_CLASS_GP, 2, false},   // Argument register
    {"x3", REG_CLASS_GP, 3, false},   // Argument register
    {"x4", REG_CLASS_GP, 4, false},   // Argument register
    {"x5", REG_CLASS_GP, 5, false},   // Argument register
    {"x6", REG_CLASS_GP, 6, false},   // Argument register
    {"x7", REG_CLASS_GP, 7, false},   // Argument register
    {"x8", REG_CLASS_GP, 8, false},   // Indirect result register
    {"x9", REG_CLASS_GP, 9, false},   // Temporary register
    {"x10", REG_CLASS_GP, 10, false}, // Temporary register
    {"x11", REG_CLASS_GP, 11, false}, // Temporary register
    {"x12", REG_CLASS_GP, 12, false}, // Temporary register
    {"x13", REG_CLASS_GP, 13, false}, // Temporary register
    {"x14", REG_CLASS_GP, 14, false}, // Temporary register
    {"x15", REG_CLASS_GP, 15, false}, // Temporary register
    {"x16", REG_CLASS_GP, 16, false}, // IP0 (intra-procedure-call)
    {"x17", REG_CLASS_GP, 17, false}, // IP1 (intra-procedure-call)
    {"x18", REG_CLASS_GP, 18, false}, // Platform register
    {"x19", REG_CLASS_GP, 19, true},  // Callee-saved
    {"x20", REG_CLASS_GP, 20, true},  // Callee-saved
    {"x21", REG_CLASS_GP, 21, true},  // Callee-saved
    {"x22", REG_CLASS_GP, 22, true},  // Callee-saved
    {"x23", REG_CLASS_GP, 23, true},  // Callee-saved
    {"x24", REG_CLASS_GP, 24, true},  // Callee-saved
    {"x25", REG_CLASS_GP, 25, true},  // Callee-saved
    {"x26", REG_CLASS_GP, 26, true},  // Callee-saved
    {"x27", REG_CLASS_GP, 27, true},  // Callee-saved
    {"x28", REG_CLASS_GP, 28, true},  // Callee-saved
    {"x29", REG_CLASS_GP, 29, true},  // Frame pointer
    {"x30", REG_CLASS_GP, 30, false}, // Link register

    // Special registers
    {"sp", REG_CLASS_SP, 31, false},  // Stack pointer
    {"pc", REG_CLASS_SP, 32, false},  // Program counter

    // SIMD registers (NEON)
    {"v0", REG_CLASS_VEC, 0, false},   // SIMD register
    {"v1", REG_CLASS_VEC, 1, false},   // SIMD register
    {"v2", REG_CLASS_VEC, 2, false},   // SIMD register
    {"v3", REG_CLASS_VEC, 3, false},   // SIMD register
    {"v4", REG_CLASS_VEC, 4, false},   // SIMD register
    {"v5", REG_CLASS_VEC, 5, false},   // SIMD register
    {"v6", REG_CLASS_VEC, 6, false},   // SIMD register
    {"v7", REG_CLASS_VEC, 7, false},   // SIMD register
    // ... (truncated for brevity, ARM64 has 32 SIMD registers)
};

#define NUM_ARM64_REGISTERS (sizeof(arm64_registers) / sizeof(TargetRegister))

// ARM64 calling convention (AArch64 ABI)
static CallingConvention arm64_calling_convention = {
    .arg_registers = NULL, // Will be initialized
    .num_arg_registers = 8,
    .return_register = &arm64_registers[0], // x0
    .stack_pointer = &arm64_registers[31],  // sp
    .frame_pointer = &arm64_registers[29],  // x29/fp
    .stack_alignment = 16,
    .caller_cleanup = false
};

// ARM64 code generation functions
static void arm64_generate_prologue(FILE* out, int stack_size) {
    emit_comment(out, "ARM64 function prologue");
    emit_instruction(out, "stp x29, x30, [sp, -16]!");  // Save FP and LR
    emit_instruction(out, "mov x29, sp");                // Set frame pointer

    if (stack_size > 0) {
        // Align stack size to 16 bytes
        int aligned_size = (stack_size + 15) & ~15;
        emit_instruction(out, "sub sp, sp, #%d", aligned_size);
    }
}

static void arm64_generate_epilogue(FILE* out, int stack_size) {
    emit_comment(out, "ARM64 function epilogue");

    if (stack_size > 0) {
        int aligned_size = (stack_size + 15) & ~15;
        emit_instruction(out, "add sp, sp, #%d", aligned_size);
    }

    emit_instruction(out, "ldp x29, x30, [sp], 16");  // Restore FP and LR
    emit_instruction(out, "ret");
}

static void arm64_generate_mov(FILE* out, const char* dest, const char* src) {
    emit_instruction(out, "mov %s, %s", dest, src);
}

static void arm64_generate_add(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "add %s, %s, %s", dest, src1, src2);
}

static void arm64_generate_sub(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "sub %s, %s, %s", dest, src1, src2);
}

static void arm64_generate_mul(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "mul %s, %s, %s", dest, src1, src2);
}

static void arm64_generate_div(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "sdiv %s, %s, %s", dest, src1, src2);  // Signed division
}

static void arm64_generate_load(FILE* out, const char* dest, const char* addr, int offset) {
    if (offset == 0) {
        emit_instruction(out, "ldr %s, [%s]", dest, addr);
    } else {
        emit_instruction(out, "ldr %s, [%s, #%d]", dest, addr, offset);
    }
}

static void arm64_generate_store(FILE* out, const char* src, const char* addr, int offset) {
    if (offset == 0) {
        emit_instruction(out, "str %s, [%s]", src, addr);
    } else {
        emit_instruction(out, "str %s, [%s, #%d]", src, addr, offset);
    }
}

static void arm64_generate_cmp(FILE* out, const char* op1, const char* op2) {
    emit_instruction(out, "cmp %s, %s", op1, op2);
}

static void arm64_generate_jmp(FILE* out, const char* label) {
    emit_instruction(out, "b %s", label);
}

static void arm64_generate_je(FILE* out, const char* label) {
    emit_instruction(out, "b.eq %s", label);
}

static void arm64_generate_jne(FILE* out, const char* label) {
    emit_instruction(out, "b.ne %s", label);
}

static void arm64_generate_jl(FILE* out, const char* label) {
    emit_instruction(out, "b.lt %s", label);
}

static void arm64_generate_jg(FILE* out, const char* label) {
    emit_instruction(out, "b.gt %s", label);
}

static void arm64_generate_call(FILE* out, const char* function) {
    emit_instruction(out, "bl %s", function);
}

static void arm64_generate_ret(FILE* out) {
    emit_instruction(out, "ret");
}

static void arm64_generate_label(FILE* out, const char* label) {
    emit_label(out, label);
}

static void arm64_apply_ia_hints(FILE* out, const char* optimization_type) {
    emit_comment(out, "ARM64 IA optimization hints");

    if (strcmp(optimization_type, "loop_unroll") == 0) {
        emit_instruction(out, ";; IA: ARM64 loop unrolling - use NEON for parallel processing");
    } else if (strcmp(optimization_type, "vectorize") == 0) {
        emit_instruction(out, ";; IA: SIMD vectorization - leverage NEON instructions");
        emit_instruction(out, ";; IA suggests: use ld1/st1 for vector loads/stores");
    } else if (strcmp(optimization_type, "cache_block") == 0) {
        emit_instruction(out, ";; IA: Cache blocking - optimize for ARM64 cache hierarchy");
        emit_instruction(out, ";; IA suggests: 64-byte cache line alignment");
    } else if (strcmp(optimization_type, "branch_predict") == 0) {
        emit_instruction(out, ";; IA: Branch prediction - ARM64 has good branch predictor");
    }
}

// ARM64 instruction set (subset for basic operations)
static TargetInstruction arm64_instructions[] = {
    {"add", 3, true},
    {"sub", 3, true},
    {"mul", 3, false},
    {"sdiv", 3, false},  // Signed divide
    {"udiv", 3, false},  // Unsigned divide
    {"ldr", 3, true},    // Load register
    {"str", 3, true},    // Store register
    {"mov", 2, true},
    {"cmp", 2, true},
    {"b", 1, false},     // Branch
    {"bl", 1, false},    // Branch and link
    {"ret", 0, false},
    {"stp", 4, false},   // Store pair
    {"ldp", 4, false},   // Load pair
};

#define NUM_ARM64_INSTRUCTIONS (sizeof(arm64_instructions) / sizeof(TargetInstruction))

// Create ARM64 backend
TargetBackend* create_arm64_backend(void) {
    TargetBackend* backend = (TargetBackend*)malloc(sizeof(TargetBackend));
    if (!backend) return NULL;

    backend->arch = TARGET_ARM64;
    backend->name = "ARM64";
    backend->triple = "aarch64-linux-gnu";

    // Allocate and copy registers
    backend->registers = (TargetRegister**)malloc(NUM_ARM64_REGISTERS * sizeof(TargetRegister*));
    for (int i = 0; i < NUM_ARM64_REGISTERS; i++) {
        backend->registers[i] = &arm64_registers[i];
    }
    backend->num_registers = NUM_ARM64_REGISTERS;

    // Set up calling convention
    backend->calling_convention = &arm64_calling_convention;
    backend->calling_convention->arg_registers = (TargetRegister**)malloc(
        backend->calling_convention->num_arg_registers * sizeof(TargetRegister*));
    for (int i = 0; i < backend->calling_convention->num_arg_registers; i++) {
        backend->calling_convention->arg_registers[i] = &arm64_registers[i];
    }

    // Allocate and copy instructions
    backend->instructions = (TargetInstruction**)malloc(NUM_ARM64_INSTRUCTIONS * sizeof(TargetInstruction*));
    for (int i = 0; i < NUM_ARM64_INSTRUCTIONS; i++) {
        backend->instructions[i] = &arm64_instructions[i];
    }
    backend->num_instructions = NUM_ARM64_INSTRUCTIONS;

    // Initialize function pointers
    backend->generate_prologue = arm64_generate_prologue;
    backend->generate_epilogue = arm64_generate_epilogue;
    backend->generate_mov = arm64_generate_mov;
    backend->generate_add = arm64_generate_add;
    backend->generate_sub = arm64_generate_sub;
    backend->generate_mul = arm64_generate_mul;
    backend->generate_div = arm64_generate_div;
    backend->generate_load = arm64_generate_load;
    backend->generate_store = arm64_generate_store;
    backend->generate_cmp = arm64_generate_cmp;
    backend->generate_jmp = arm64_generate_jmp;
    backend->generate_je = arm64_generate_je;
    backend->generate_jne = arm64_generate_jne;
    backend->generate_jl = arm64_generate_jl;
    backend->generate_jg = arm64_generate_jg;
    backend->generate_call = arm64_generate_call;
    backend->generate_ret = arm64_generate_ret;
    backend->generate_label = arm64_generate_label;
    backend->apply_ia_hints = arm64_apply_ia_hints;

    return backend;
}
