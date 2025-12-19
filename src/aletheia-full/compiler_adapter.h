/*
 * ALETHEIA-Full: Compiler Adapter
 *
 * Adapts the existing ALETHEIA compiler for bootstrap compilation.
 * This allows the full-featured compiler to be built using ALETHEIA-Core.
 */

#ifndef COMPILER_ADAPTER_H
#define COMPILER_ADAPTER_H

#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* Bootstrap compatibility layer */
/* These definitions ensure compatibility with ALETHEIA-Core limitations */

#ifdef BOOTSTRAP_BUILD
/* Simplified types for bootstrap compatibility */
#define bool int
#define true 1
#define false 0

/* Memory management */
void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);

/* String functions */
size_t strlen(const char* s);
char* strcpy(char* dest, const char* src);
char* strdup(const char* s);
int strcmp(const char* s1, const char* s2);
char* strstr(const char* haystack, const char* needle);

/* File I/O - using standard stdio.h */

/* Standard library functions */
void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);

/* Math functions (basic) */
double sqrt(double x);
double pow(double base, double exp);

/* Time functions */
typedef long time_t;
time_t time(time_t* t);

/* Exit and abort */
void exit(int status);
void abort(void);

#else
/* Normal compilation - include standard headers */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#endif

/* ALETHEIA-Full specific features */
#define ALETHEIA_FULL_VERSION "1.0.0-full"

/* AI Optimization levels */
typedef enum {
    AI_OPT_NONE = 0,
    AI_OPT_BASIC,
    AI_OPT_ADVANCED,
    AI_OPT_AGGRESSIVE
} AIOptimizationLevel;

/* Compiler configuration for ALETHEIA-Full */
typedef struct {
    /* Basic options */
    int optimization_level;
    int debug_info;
    int verbose;
    const char* output_file;
    const char* input_file;

    /* AI features */
    AIOptimizationLevel ai_level;
    int learning_enabled;
    const char* model_file;

    /* Advanced features */
    int security_scan;
    int performance_analysis;
    const char* target_arch;

    /* Bootstrap compatibility */
    int bootstrap_mode;
} ALETHEIAConfig;

/* Main compilation functions */
int aletheia_compile(const char* source, size_t length, ALETHEIAConfig* config);
int aletheia_compile_file(const char* input_file, const char* output_file, ALETHEIAConfig* config);

/* AI-powered optimization functions */
void ai_optimize_ast(void* ast, AIOptimizationLevel level);
void ai_predict_optimizations(const char* source, ALETHEIAConfig* config);
void ai_learn_from_compilation(const char* source, const char* result);

/* Security analysis */
void security_scan_code(const char* source);
void vulnerability_detect(const char* source);

/* Advanced diagnostics */
void performance_analyze(const char* source);
void code_quality_report(const char* source);

/* Multi-target support */
int supports_architecture(const char* arch);
void set_target_architecture(const char* arch);

#endif /* COMPILER_ADAPTER_H */

