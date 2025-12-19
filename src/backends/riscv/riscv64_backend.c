// ALETHEIA RISC-V 64 Backend Implementation
// RV64G code generation for IoT and embedded platforms

#include "../backend.h"
#include <stdlib.h>
#include <string.h>

// RISC-V 64 registers
static TargetRegister riscv64_registers[] = {
    // General purpose registers (x0-x31)
    {"zero", REG_CLASS_GP, 0, false},  // Hardwired zero
    {"ra", REG_CLASS_GP, 1, false},    // Return address
    {"sp", REG_CLASS_GP, 2, false},    // Stack pointer
    {"gp", REG_CLASS_GP, 3, false},    // Global pointer
    {"tp", REG_CLASS_GP, 4, false},    // Thread pointer
    {"t0", REG_CLASS_GP, 5, false},    // Temporary/alternate link register
    {"t1", REG_CLASS_GP, 6, false},    // Temporary
    {"t2", REG_CLASS_GP, 7, false},    // Temporary
    {"s0", REG_CLASS_GP, 8, true},     // Saved register/frame pointer
    {"s1", REG_CLASS_GP, 9, true},     // Saved register
    {"a0", REG_CLASS_GP, 10, false},   // Function argument/return value
    {"a1", REG_CLASS_GP, 11, false},   // Function argument/return value
    {"a2", REG_CLASS_GP, 12, false},   // Function argument
    {"a3", REG_CLASS_GP, 13, false},   // Function argument
    {"a4", REG_CLASS_GP, 14, false},   // Function argument
    {"a5", REG_CLASS_GP, 15, false},   // Function argument
    {"a6", REG_CLASS_GP, 16, false},   // Function argument
    {"a7", REG_CLASS_GP, 17, false},   // Function argument
    {"s2", REG_CLASS_GP, 18, true},    // Saved register
    {"s3", REG_CLASS_GP, 19, true},    // Saved register
    {"s4", REG_CLASS_GP, 20, true},    // Saved register
    {"s5", REG_CLASS_GP, 21, true},    // Saved register
    {"s6", REG_CLASS_GP, 22, true},    // Saved register
    {"s7", REG_CLASS_GP, 23, true},    // Saved register
    {"s8", REG_CLASS_GP, 24, true},    // Saved register
    {"s9", REG_CLASS_GP, 25, true},    // Saved register
    {"s10", REG_CLASS_GP, 26, true},   // Saved register
    {"s11", REG_CLASS_GP, 27, true},   // Saved register
    {"t3", REG_CLASS_GP, 28, false},   // Temporary
    {"t4", REG_CLASS_GP, 29, false},   // Temporary
    {"t5", REG_CLASS_GP, 30, false},   // Temporary
    {"t6", REG_CLASS_GP, 31, false},   // Temporary

    // Floating point registers (f0-f31) - RV64G includes D extension
    {"ft0", REG_CLASS_VEC, 0, false},  // FP temporary
    {"ft1", REG_CLASS_VEC, 1, false},  // FP temporary
    {"ft2", REG_CLASS_VEC, 2, false},  // FP temporary
    {"ft3", REG_CLASS_VEC, 3, false},  // FP temporary
    {"ft4", REG_CLASS_VEC, 4, false},  // FP temporary
    {"ft5", REG_CLASS_VEC, 5, false},  // FP temporary
    {"ft6", REG_CLASS_VEC, 6, false},  // FP temporary
    {"ft7", REG_CLASS_VEC, 7, false},  // FP temporary
    {"fs0", REG_CLASS_VEC, 8, true},   // FP saved register
    {"fs1", REG_CLASS_VEC, 9, true},   // FP saved register
    {"fa0", REG_CLASS_VEC, 10, false}, // FP argument/return value
    {"fa1", REG_CLASS_VEC, 11, false}, // FP argument/return value
    {"fa2", REG_CLASS_VEC, 12, false}, // FP argument
    {"fa3", REG_CLASS_VEC, 13, false}, // FP argument
    {"fa4", REG_CLASS_VEC, 14, false}, // FP argument
    {"fa5", REG_CLASS_VEC, 15, false}, // FP argument
    {"fa6", REG_CLASS_VEC, 16, false}, // FP argument
    {"fa7", REG_CLASS_VEC, 17, false}, // FP argument
    {"fs2", REG_CLASS_VEC, 18, true},  // FP saved register
    {"fs3", REG_CLASS_VEC, 19, true},  // FP saved register
    {"fs4", REG_CLASS_VEC, 20, true},  // FP saved register
    {"fs5", REG_CLASS_VEC, 21, true},  // FP saved register
    {"fs6", REG_CLASS_VEC, 22, true},  // FP saved register
    {"fs7", REG_CLASS_VEC, 23, true},  // FP saved register
    {"fs8", REG_CLASS_VEC, 24, true},  // FP saved register
    {"fs9", REG_CLASS_VEC, 25, true},  // FP saved register
    {"fs10", REG_CLASS_VEC, 26, true}, // FP saved register
    {"fs11", REG_CLASS_VEC, 27, true}, // FP saved register
    {"ft8", REG_CLASS_VEC, 28, false}, // FP temporary
    {"ft9", REG_CLASS_VEC, 29, false}, // FP temporary
    {"ft10", REG_CLASS_VEC, 30, false}, // FP temporary
    {"ft11", REG_CLASS_VEC, 31, false}, // FP temporary
};

#define NUM_RISCV64_REGISTERS (sizeof(riscv64_registers) / sizeof(TargetRegister))

// RISC-V calling convention (Linux ABI)
static CallingConvention riscv64_calling_convention = {
    .arg_registers = NULL, // Will be initialized
    .num_arg_registers = 8,
    .return_register = &riscv64_registers[10], // a0
    .stack_pointer = &riscv64_registers[2],    // sp
    .frame_pointer = &riscv64_registers[8],    // s0
    .stack_alignment = 16,
    .caller_cleanup = false
};

// RISC-V code generation functions
static void riscv64_generate_prologue(FILE* out, int stack_size) {
    emit_comment(out, "RISC-V function prologue");
    emit_instruction(out, "addi sp, sp, -16");  // Allocate stack space
    emit_instruction(out, "sd ra, 8(sp)");      // Save return address
    emit_instruction(out, "sd s0, 0(sp)");      // Save frame pointer
    emit_instruction(out, "addi s0, sp, 16");    // Set frame pointer

    if (stack_size > 0) {
        // Align stack size to 16 bytes
        int aligned_size = (stack_size + 15) & ~15;
        emit_instruction(out, "addi sp, sp, -%d", aligned_size);
    }
}

static void riscv64_generate_epilogue(FILE* out, int stack_size) {
    emit_comment(out, "RISC-V function epilogue");

    if (stack_size > 0) {
        int aligned_size = (stack_size + 15) & ~15;
        emit_instruction(out, "addi sp, sp, %d", aligned_size);
    }

    emit_instruction(out, "ld s0, 0(sp)");      // Restore frame pointer
    emit_instruction(out, "ld ra, 8(sp)");      // Restore return address
    emit_instruction(out, "addi sp, sp, 16");   // Deallocate stack space
    emit_instruction(out, "ret");
}

static void riscv64_generate_mov(FILE* out, const char* dest, const char* src) {
    emit_instruction(out, "mv %s, %s", dest, src);
}

static void riscv64_generate_add(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "add %s, %s, %s", dest, src1, src2);
}

static void riscv64_generate_sub(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "sub %s, %s, %s", dest, src1, src2);
}

static void riscv64_generate_mul(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "mul %s, %s, %s", dest, src1, src2);
}

static void riscv64_generate_div(FILE* out, const char* dest, const char* src1, const char* src2) {
    emit_instruction(out, "div %s, %s, %s", dest, src1, src2);  // Signed division
}

// BUG FIX 1: Corrected format specifier order
// Format string "ld %s, %d(%s)" expects (dest, offset, addr)
// Was receiving (dest, addr, offset) causing %d to format string and %s to format int
static void riscv64_generate_load(FILE* out, const char* dest, const char* addr, int offset) {
    if (offset == 0) {
        emit_instruction(out, "ld %s, 0(%s)", dest, addr);
    } else {
        emit_instruction(out, "ld %s, %d(%s)", dest, offset, addr);
    }
}

// BUG FIX 1: Corrected format specifier order for store as well
static void riscv64_generate_store(FILE* out, const char* src, const char* addr, int offset) {
    if (offset == 0) {
        emit_instruction(out, "sd %s, 0(%s)", src, addr);
    } else {
        emit_instruction(out, "sd %s, %d(%s)", src, offset, addr);
    }
}

static void riscv64_generate_cmp(FILE* out, const char* op1, const char* op2) {
    // RISC-V doesn't have a direct compare instruction
    // Comparison is done with branches or using slt
    emit_instruction(out, "slt t0, %s, %s", op1, op2);  // Set if less than
}

static void riscv64_generate_jmp(FILE* out, const char* label) {
    emit_instruction(out, "j %s", label);
}

// BUG FIX 2: Changed from unconditional branch to proper conditional jump
// Original: "beq zero, zero, label" (always true - unconditional)
// Fixed: "beq t0, zero, label" (conditional based on comparison result in t0)
static void riscv64_generate_je(FILE* out, const char* label) {
    emit_comment(out, "RISC-V conditional jump if equal (based on previous comparison)");
    emit_instruction(out, "beq t0, zero, %s", label);  // Branch if t0 == 0 (comparison result)
}

static void riscv64_generate_jne(FILE* out, const char* label) {
    // This is tricky in RISC-V - we'd need to set up condition flags
    emit_comment(out, "JNE requires condition setup in RISC-V");
    emit_instruction(out, "j %s", label);  // Placeholder
}

static void riscv64_generate_jl(FILE* out, const char* label) {
    emit_instruction(out, "blt t0, zero, %s", label);
}

static void riscv64_generate_jg(FILE* out, const char* label) {
    emit_instruction(out, "bgt t0, zero, %s", label);
}

static void riscv64_generate_call(FILE* out, const char* function) {
    emit_instruction(out, "call %s", function);
}

static void riscv64_generate_ret(FILE* out) {
    emit_instruction(out, "ret");
}

static void riscv64_generate_label(FILE* out, const char* label) {
    emit_label(out, label);
}

static void riscv64_apply_ia_hints(FILE* out, const char* optimization_type) {
    emit_comment(out, "RISC-V IA optimization hints");

    if (strcmp(optimization_type, "loop_unroll") == 0) {
        emit_instruction(out, ";; IA: RISC-V loop unrolling - use compressed instructions");
    } else if (strcmp(optimization_type, "vectorize") == 0) {
        emit_instruction(out, ";; IA: SIMD vectorization - future RV64V extension");
        emit_instruction(out, ";; IA suggests: prepare for vector instructions");
    } else if (strcmp(optimization_type, "cache_block") == 0) {
        emit_instruction(out, ";; IA: Cache blocking - optimize for RISC-V cache");
        emit_instruction(out, ";; IA suggests: 64-byte cache line alignment");
    } else if (strcmp(optimization_type, "branch_predict") == 0) {
        emit_instruction(out, ";; IA: Branch prediction - RISC-V has simple predictor");
        emit_instruction(out, ";; IA suggests: minimize branches when possible");
    }
}

// RISC-V instruction set (RV64G subset)
static TargetInstruction riscv64_instructions[] = {
    {"add", 3, false},
    {"addi", 3, true},   // Add immediate
    {"sub", 3, false},
    {"mul", 3, false},
    {"div", 3, false},   // Signed divide
    {"rem", 3, false},   // Remainder
    {"slt", 3, false},   // Set less than
    {"ld", 3, true},     // Load doubleword
    {"sd", 3, true},     // Store doubleword
    {"lw", 3, true},     // Load word
    {"sw", 3, true},     // Store word
    {"mv", 2, false},    // Move (pseudo-instruction)
    {"li", 2, true},     // Load immediate (pseudo-instruction)
    {"j", 1, false},     // Jump
    {"jal", 2, false},   // Jump and link
    {"jalr", 3, false},  // Jump and link register
    {"beq", 3, false},   // Branch if equal
    {"bne", 3, false},   // Branch if not equal
    {"blt", 3, false},   // Branch if less than
    {"bge", 3, false},   // Branch if greater or equal
    {"ret", 0, false},   // Return (pseudo-instruction)
    {"call", 1, false},  // Call function (pseudo-instruction)
};

#define NUM_RISCV64_INSTRUCTIONS (sizeof(riscv64_instructions) / sizeof(TargetInstruction))

// Create RISC-V 64 backend
TargetBackend* create_riscv64_backend(void) {
    TargetBackend* backend = (TargetBackend*)malloc(sizeof(TargetBackend));
    if (!backend) return NULL;

    backend->arch = TARGET_RISCV64;
    backend->name = "RISC-V 64";
    backend->triple = "riscv64-linux-gnu";

    // Allocate and copy registers
    backend->registers = (TargetRegister**)malloc(NUM_RISCV64_REGISTERS * sizeof(TargetRegister*));
    for (int i = 0; i < NUM_RISCV64_REGISTERS; i++) {
        backend->registers[i] = &riscv64_registers[i];
    }
    backend->num_registers = NUM_RISCV64_REGISTERS;

    // Set up calling convention
    backend->calling_convention = &riscv64_calling_convention;
    backend->calling_convention->arg_registers = (TargetRegister**)malloc(
        backend->calling_convention->num_arg_registers * sizeof(TargetRegister*));
    for (int i = 0; i < backend->calling_convention->num_arg_registers; i++) {
        // Map to a0-a7 registers (indices 10-17 in our array)
        backend->calling_convention->arg_registers[i] = &riscv64_registers[10 + i];
    }

    // Allocate and copy instructions
    backend->instructions = (TargetInstruction**)malloc(NUM_RISCV64_INSTRUCTIONS * sizeof(TargetInstruction*));
    for (int i = 0; i < NUM_RISCV64_INSTRUCTIONS; i++) {
        backend->instructions[i] = &riscv64_instructions[i];
    }
    backend->num_instructions = NUM_RISCV64_INSTRUCTIONS;

    // Initialize function pointers
    backend->generate_prologue = riscv64_generate_prologue;
    backend->generate_epilogue = riscv64_generate_epilogue;
    backend->generate_mov = riscv64_generate_mov;
    backend->generate_add = riscv64_generate_add;
    backend->generate_sub = riscv64_generate_sub;
    backend->generate_mul = riscv64_generate_mul;
    backend->generate_div = riscv64_generate_div;
    backend->generate_load = riscv64_generate_load;
    backend->generate_store = riscv64_generate_store;
    backend->generate_cmp = riscv64_generate_cmp;
    backend->generate_jmp = riscv64_generate_jmp;
    backend->generate_je = riscv64_generate_je;
    backend->generate_jne = riscv64_generate_jne;
    backend->generate_jl = riscv64_generate_jl;
    backend->generate_jg = riscv64_generate_jg;
    backend->generate_call = riscv64_generate_call;
    backend->generate_ret = riscv64_generate_ret;
    backend->generate_label = riscv64_generate_label;
    backend->apply_ia_hints = riscv64_apply_ia_hints;

    return backend;
}