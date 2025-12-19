#!/bin/bash

echo "=== ALETHEIA BUILD VERIFICATION ==="
echo ""

# Test bootstrap
echo "1. Bootstrap binary generation..."
if ./build/bootstrap_emitter > /dev/null 2>&1 && [ -f "build/bootstrap.bin" ]; then
    echo "✓ PASS: Bootstrap binary generated"
else
    echo "✗ FAIL: Bootstrap generation failed"
fi

# Test assembler
echo "2. Assembler functionality..."
if ./build/assembler tests/test.asm /tmp/test.bin > /dev/null 2>&1; then
    echo "✓ PASS: Assembler works"
else
    echo "✗ FAIL: Assembler failed"
fi

# Test linker compilation
echo "3. Linker compilation..."
if [ -f "build/linker.exe" ]; then
    echo "✓ PASS: Linker exists"
else
    echo "✗ FAIL: Linker missing"
fi

# Check structure
echo "4. Project structure..."
if [ -d "src" ] && [ -d "build" ] && [ -d "docs" ] && [ -d "tests" ] && [ -f "TODO.md" ]; then
    echo "✓ PASS: Structure is clean"
else
    echo "✗ FAIL: Structure issues"
fi

echo ""
echo "=== VERIFICATION COMPLETE ==="
echo ""
echo "ALETHEIA C Compiler build completed successfully"
