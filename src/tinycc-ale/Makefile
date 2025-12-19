# ALETHEIA TinyCC-ALE Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -I.

# Source files
SRC = main.c lexer.c parser.c codegen.c
OBJ = $(SRC:.c=.o)
TARGET = tinycc-ale

# Build the compiler
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Compile object files
%.o: %.c tinycc.h
	$(CC) $(CFLAGS) -c $< -o $@

# Test the compiler
test: $(TARGET)
	@echo "Testing TinyCC-ALE..."
	@echo "int main() { return 42; }" | ./$(TARGET) > test_output.asm
	@echo "Generated assembly:"
	@cat test_output.asm

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET) test_output.asm

.PHONY: test clean

