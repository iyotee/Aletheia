/* ALETHEIA-Core: Bootstrap C Compiler - Simplified for TinyCC-ALE */

/* Basic types */
typedef int bool;
#define true 1
#define false 0

/* Memory management (simplified) */
void* core_malloc(int size);

/* Error reporting */
void core_error();

/* Compiler phases */
int compile_source(char* source);
