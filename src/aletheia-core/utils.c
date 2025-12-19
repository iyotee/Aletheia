/* ALETHEIA-Core: Basic Utilities - Simplified for TinyCC-ALE */

#include "core.h"

/* Simple memory pool */
static char memory_pool[65536]; /* 64KB pool */
static int memory_used = 0;

void* core_malloc(int size) {
    if (memory_used + size > sizeof(memory_pool)) {
        return 0; /* Out of memory */
    }
    void* ptr = &memory_pool[memory_used];
    memory_used += size;
    return ptr;
}

void core_error() {
    /* Simple error - just exit */
}
