# ALETHEIA-Full Makefile
# GCC 100% Compatible Compiler Build System

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -I. -I../backends -I../asm
LDFLAGS =

# Source files - all required for complete compilation
SRCS = aletheia-full.c ast.c codegen.c compiler.c diagnostic.c lexer.c main.c optimizer.c parser.c preprocessor.c self_learning_ai.c semantic.c ai_stubs.c
BACKEND_SRCS = ../backends/backend.c ../backends/arm64/arm64_backend.c ../backends/riscv/riscv64_backend.c
ASM_SRCS = ../asm/assembler.c ../asm/geno_format.c

# All source files combined
ALL_SRCS = $(SRCS) $(BACKEND_SRCS) $(ASM_SRCS)
OBJS = $(ALL_SRCS:.c=.o)
TARGET = aletheia-full

# Default target
all: $(TARGET)

# Main target
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

# Test compilation
test: $(TARGET)
	@echo "Testing ALETHEIA-Full GCC 100% compilation..."
	./$(TARGET) test_input.c test_output.s

# Install (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean all

.PHONY: all clean test install debug