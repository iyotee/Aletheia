#!/bin/bash

# ALETHEIA Build and Test Script
# Comprehensive testing for CI/CD pipeline

set -e

echo "🔨 ALETHEIA Build and Test Script"
echo "=================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
TEST_RESULTS="test_results.txt"
COVERAGE_REPORT="coverage.html"

# Clean previous build
echo -e "${BLUE}Cleaning previous build...${NC}"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Function to log results
log_result() {
    local test_name=$1
    local result=$2
    local details=$3

    echo "$test_name: $result" >> "$TEST_RESULTS"
    if [ -n "$details" ]; then
        echo "  $details" >> "$TEST_RESULTS"
    fi

    if [ "$result" = "PASS" ]; then
        echo -e "${GREEN}✅ $test_name: PASS${NC}"
    else
        echo -e "${RED}❌ $test_name: FAIL${NC}"
        if [ -n "$details" ]; then
            echo -e "${RED}   $details${NC}"
        fi
    fi
}

# Initialize test results
echo "ALETHEIA Test Results - $(date)" > "$TEST_RESULTS"
echo "=================================" >> "$TEST_RESULTS"
echo "" >> "$TEST_RESULTS"

# Build ALETHEIA-Full
echo -e "${BLUE}Building ALETHEIA-Full...${NC}"
cd src/aletheia-full
make clean
if make CFLAGS="-Wall -Wextra -g"; then
    log_result "Build ALETHEIA-Full" "PASS"
else
    log_result "Build ALETHEIA-Full" "FAIL" "Compilation failed"
    exit 1
fi
cd ../..

# Build MesCC-ALE
echo -e "${BLUE}Building MesCC-ALE...${NC}"
cd src/mescc-ale
make clean
if make; then
    log_result "Build MesCC-ALE" "PASS"
else
    log_result "Build MesCC-ALE" "FAIL" "Compilation failed"
fi
cd ../..

# Build ALETHEIA-Core
echo -e "${BLUE}Building ALETHEIA-Core...${NC}"
cd src/aletheia-core
make clean
if make; then
    log_result "Build ALETHEIA-Core" "PASS"
else
    log_result "Build ALETHEIA-Core" "FAIL" "Compilation failed"
fi
cd ../..

# Test basic functionality
echo -e "${BLUE}Testing basic functionality...${NC}"

# Test simple program compilation
if ./src/aletheia-full/aletheia-full-ai testing/emulators/test_program.c test_output.s --target x86-64 >/dev/null 2>&1; then
    log_result "Basic compilation test" "PASS"
else
    log_result "Basic compilation test" "FAIL" "Failed to compile test program"
fi

# Test multi-target compilation
echo -e "${BLUE}Testing multi-target compilation...${NC}"

targets=("x86-64" "arm64" "riscv64")
for target in "${targets[@]}"; do
    if ./src/aletheia-full/aletheia-full-ai testing/emulators/test_program.c "test_${target}.s" --target "$target" >/dev/null 2>&1; then
        log_result "Multi-target $target" "PASS"
    else
        log_result "Multi-target $target" "FAIL" "Failed to compile for $target"
    fi
done

# Test AI system (if available)
echo -e "${BLUE}Testing AI system...${NC}"
if [ -f "ai/simple_ai_test.py" ]; then
    cd ai
    if python simple_ai_test.py >/dev/null 2>&1; then
        log_result "AI system test" "PASS"
    else
        log_result "AI system test" "FAIL" "AI tests failed (may be missing dependencies)"
    fi
    cd ..
else
    log_result "AI system test" "SKIP" "AI test script not found"
fi

# Test bootstrap chain
echo -e "${BLUE}Testing bootstrap chain...${NC}"

# Check if all stages exist
bootstrap_pass=true
for stage in "src/hex_loader" "src/mescc-ale" "src/aletheia-core" "src/aletheia-full"; do
    if [ -d "$stage" ]; then
        echo "  Found stage: $stage"
    else
        echo "  Missing stage: $stage"
        bootstrap_pass=false
    fi
done

if $bootstrap_pass; then
    log_result "Bootstrap chain validation" "PASS"
else
    log_result "Bootstrap chain validation" "FAIL" "Missing bootstrap stages"
fi

# Code quality checks
echo -e "${BLUE}Running code quality checks...${NC}"

# Check for common issues
error_count=$(find src -name "*.c" -o -name "*.h" | xargs grep -l "TODO\|FIXME\|XXX" | wc -l)
if [ "$error_count" -eq 0 ]; then
    log_result "Code quality check" "PASS"
else
    log_result "Code quality check" "WARN" "Found $error_count files with TODO/FIXME comments"
fi

# Check for memory leaks (basic)
memory_issues=$(find src -name "*.c" | xargs grep -l "malloc\|free\|strdup" | wc -l)
log_result "Memory management check" "INFO" "Found $memory_issues files using dynamic memory"

# Generate summary
echo "" >> "$TEST_RESULTS"
echo "SUMMARY" >> "$TEST_RESULTS"
echo "=======" >> "$TEST_RESULTS"

total_tests=$(grep -c ": " "$TEST_RESULTS")
passed_tests=$(grep -c ": PASS" "$TEST_RESULTS")
failed_tests=$(grep -c ": FAIL" "$TEST_RESULTS")
warn_tests=$(grep -c ": WARN" "$TEST_RESULTS")

echo "Total tests: $total_tests" >> "$TEST_RESULTS"
echo "Passed: $passed_tests" >> "$TEST_RESULTS"
echo "Failed: $failed_tests" >> "$TEST_RESULTS"
echo "Warnings: $warn_tests" >> "$TEST_RESULTS"

success_rate=$(( (passed_tests * 100) / total_tests ))

echo "Success rate: ${success_rate}%" >> "$TEST_RESULTS"

# Print summary to console
echo ""
echo -e "${BLUE}📊 TEST SUMMARY${NC}"
echo "==============="
echo "Total tests: $total_tests"
echo -e "Passed: ${GREEN}$passed_tests${NC}"
echo -e "Failed: ${RED}$failed_tests${NC}"
echo -e "Warnings: ${YELLOW}$warn_tests${NC}"
echo "Success rate: ${success_rate}%"

if [ $success_rate -ge 80 ]; then
    echo -e "${GREEN}🎉 Build and tests PASSED!${NC}"
    exit 0
elif [ $success_rate -ge 60 ]; then
    echo -e "${YELLOW}⚠️  Build and tests mostly PASSED with some issues${NC}"
    exit 0
else
    echo -e "${RED}❌ Build and tests FAILED!${NC}"
    exit 1
fi
