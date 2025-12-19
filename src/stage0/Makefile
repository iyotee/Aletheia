# ALETHEIA Stage 0 Makefile
# The absolute foundation - creates hex monitor from mathematical foundations

CC=gcc
CFLAGS=-O0 -Wall -Wextra -std=c99

# Default target: build the hex monitor
all: hex0.bin

# Stage 0: Mathematical foundations
# We need GCC to compile the hex emitter (this is the ONLY GCC usage in the entire chain)
hex0: hex0.c
	$(CC) $(CFLAGS) -o hex0 hex0.c

# The hex monitor binary (127 bytes of pure machine code)
hex0.bin: hex0
	./hex0 > hex0.bin
	chmod +x hex0.bin

# Test the hex monitor
test: hex0.bin
	@echo "Testing Stage 0 hex monitor..."
	@./hex0.bin < test_return_42.hex; echo "Exit code: $$?"

# Clean build artifacts
clean:
	-del /Q hex0 hex0.bin *.o *.exe 2>nul || true

# Full clean including all generated files
distclean: clean
	-del /Q mescc-ale.hex test_*.hex *.bin *.exe 2>nul || true

.PHONY: all test clean distclean
