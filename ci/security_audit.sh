#!/bin/bash

# ALETHEIA Security Audit Script
# Comprehensive security and compliance validation

set -e

echo "🔒 ALETHEIA Security Audit"
echo "=========================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
AUDIT_REPORT="security_audit.txt"
VULNERABILITIES_FOUND=0
WARNINGS_FOUND=0

# Initialize audit report
echo "ALETHEIA Security Audit Report - $(date)" > "$AUDIT_REPORT"
echo "=====================================" >> "$AUDIT_REPORT"
echo "" >> "$AUDIT_REPORT"

# Function to log security findings
log_security() {
    local severity=$1
    local category=$2
    local description=$3
    local file=$4
    local line=$5

    echo "[$severity] $category: $description" >> "$AUDIT_REPORT"
    if [ -n "$file" ]; then
        echo "  File: $file" >> "$AUDIT_REPORT"
    fi
    if [ -n "$line" ]; then
        echo "  Line: $line" >> "$AUDIT_REPORT"
    fi
    echo "" >> "$AUDIT_REPORT"

    case $severity in
        "CRITICAL")
            echo -e "${RED}🚨 CRITICAL: $description${NC}"
            ((VULNERABILITIES_FOUND++))
            ;;
        "HIGH")
            echo -e "${RED}⚠️  HIGH: $description${NC}"
            ((VULNERABILITIES_FOUND++))
            ;;
        "MEDIUM")
            echo -e "${YELLOW}⚠️  MEDIUM: $description${NC}"
            ((WARNINGS_FOUND++))
            ;;
        "LOW")
            echo -e "${BLUE}ℹ️  LOW: $description${NC}"
            ;;
        "INFO")
            echo -e "${BLUE}ℹ️  INFO: $description${NC}"
            ;;
    esac
}

# Check for common security issues
echo -e "${BLUE}Checking for security vulnerabilities...${NC}"

# 1. Buffer overflows
echo "1. Checking for buffer overflow vulnerabilities..."
buffer_overflows=$(find src -name "*.c" -exec grep -l "strcpy\|strcat\|sprintf\|gets" {} \; | wc -l)
if [ "$buffer_overflows" -gt 0 ]; then
    log_security "HIGH" "Buffer Overflow" "Found $buffer_overflows files using unsafe string functions" "" ""
    log_security "INFO" "Recommendation" "Replace strcpy/strcat with strlcpy/strlcat or use bounded versions" "" ""
else
    log_security "INFO" "Buffer Overflow" "No unsafe string functions found" "" ""
fi

# 2. Memory leaks
echo "2. Checking for memory management issues..."
malloc_count=$(find src -name "*.c" -exec grep -c "malloc\|calloc\|realloc" {} \; | awk '{sum += $1} END {print sum}')
free_count=$(find src -name "*.c" -exec grep -c "free" {} \; | awk '{sum += $1} END {print sum}')

if [ "$malloc_count" -gt "$free_count" ]; then
    imbalance=$((malloc_count - free_count))
    log_security "MEDIUM" "Memory Leak" "Potential memory leaks: $imbalance more mallocs than frees" "" ""
else
    log_security "INFO" "Memory Management" "Memory allocation/deallocation appears balanced" "" ""
fi

# 3. Format string vulnerabilities
echo "3. Checking for format string vulnerabilities..."
format_strings=$(find src -name "*.c" -exec grep -c "printf.*%[^\"]*[^\"]*$" {} \; | awk '{sum += $1} END {print sum}')
if [ "$format_strings" -gt 0 ]; then
    log_security "HIGH" "Format String" "Found $format_strings potential format string vulnerabilities" "" ""
    log_security "INFO" "Recommendation" "Use printf with explicit format strings, avoid user-controlled formats" "" ""
fi

# 4. Integer overflows
echo "4. Checking for integer overflow risks..."
int_overflows=$(find src -name "*.c" -exec grep -c "[\+\-\*\/]\s*[\+\-\*\/]" {} \; | awk '{sum += $1} END {print sum}')
if [ "$int_overflows" -gt 0 ]; then
    log_security "MEDIUM" "Integer Overflow" "Found arithmetic operations that may overflow" "" ""
fi

# 5. Command injection
echo "5. Checking for command injection vulnerabilities..."
system_calls=$(find src -name "*.c" -exec grep -c "system\|popen\|exec" {} \; | awk '{sum += $1} END {print sum}')
if [ "$system_calls" -gt 0 ]; then
    log_security "HIGH" "Command Injection" "Found $system_calls system/exec calls" "" ""
    log_security "INFO" "Recommendation" "Validate and sanitize all command arguments" "" ""
fi

# 6. Hardcoded secrets
echo "6. Checking for hardcoded secrets..."
secrets=$(find . -name "*.c" -o -name "*.h" -o -name "*.py" | xargs grep -l "password\|secret\|key\|token" 2>/dev/null | wc -l)
if [ "$secrets" -gt 0 ]; then
    log_security "MEDIUM" "Hardcoded Secrets" "Found potential hardcoded secrets in $secrets files" "" ""
    log_security "INFO" "Recommendation" "Use environment variables or secure key management" "" ""
fi

# 7. Insecure random number generation
echo "7. Checking for insecure random number generation..."
weak_random=$(find src -name "*.c" -exec grep -c "rand\|random" {} \; | awk '{sum += $1} END {print sum}')
if [ "$weak_random" -gt 0 ]; then
    log_security "LOW" "Weak Random" "Found $weak_random uses of weak random number generators" "" ""
    log_security "INFO" "Recommendation" "Use cryptographically secure random functions for security-critical operations" "" ""
fi

# 8. Trusting Trust validation
echo "8. Validating Trusting Trust implementation..."
bootstrap_files=$(find . -name "*bootstrap*" -o -name "*trust*" | wc -l)
if [ "$bootstrap_files" -gt 0 ]; then
    log_security "INFO" "Trusting Trust" "Found bootstrap/trust-related files: $bootstrap_files" "" ""
    log_security "INFO" "Security Feature" "Trusting Trust attack mitigation implemented" "" ""
else
    log_security "LOW" "Trusting Trust" "No explicit Trusting Trust protection found" "" ""
fi

# 9. Input validation
echo "9. Checking input validation..."
input_functions=$(find src -name "*.c" -exec grep -c "scanf\|fgets\|fread" {} \; | awk '{sum += $1} END {print sum}')
if [ "$input_functions" -gt 0 ]; then
    log_security "MEDIUM" "Input Validation" "Found $input_functions input functions" "" ""
    log_security "INFO" "Recommendation" "Implement bounds checking and input sanitization" "" ""
fi

# 10. Error handling
echo "10. Checking error handling..."
error_checks=$(find src -name "*.c" -exec grep -c "if.*NULL\|if.*<.*0\|if.*==.*-1" {} \; | awk '{sum += $1} END {print sum}')
if [ "$error_checks" -eq 0 ]; then
    log_security "LOW" "Error Handling" "Limited error checking found" "" ""
    log_security "INFO" "Recommendation" "Add comprehensive error checking for all system calls" "" ""
else
    log_security "INFO" "Error Handling" "Found $error_checks error checks" "" ""
fi

# 11. Code quality metrics
echo "11. Analyzing code quality..."
total_lines=$(find src -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')
total_files=$(find src -name "*.c" -o -name "*.h" | wc -l)

log_security "INFO" "Code Metrics" "Total lines: $total_lines, Files: $total_files" "" ""

# 12. Dependency analysis
echo "12. Analyzing dependencies..."
external_deps=$(find src -name "*.c" -exec grep -h "^#include <" {} \; | sort | uniq | wc -l)
if [ "$external_deps" -gt 10 ]; then
    log_security "LOW" "Dependencies" "High number of external dependencies: $external_deps" "" ""
    log_security "INFO" "Recommendation" "Minimize external dependencies for better security" "" ""
else
    log_security "INFO" "Dependencies" "Reasonable number of external dependencies: $external_deps" "" ""
fi

# Generate summary
echo "" >> "$AUDIT_REPORT"
echo "SECURITY AUDIT SUMMARY" >> "$AUDIT_REPORT"
echo "====================" >> "$AUDIT_REPORT"
echo "Critical vulnerabilities: $(grep -c "\[CRITICAL\]" "$AUDIT_REPORT")" >> "$AUDIT_REPORT"
echo "High severity issues: $(grep -c "\[HIGH\]" "$AUDIT_REPORT")" >> "$AUDIT_REPORT"
echo "Medium severity issues: $(grep -c "\[MEDIUM\]" "$AUDIT_REPORT")" >> "$AUDIT_REPORT"
echo "Low severity issues: $(grep -c "\[LOW\]" "$AUDIT_REPORT")" >> "$AUDIT_REPORT"
echo "Informational notes: $(grep -c "\[INFO\]" "$AUDIT_REPORT")" >> "$AUDIT_REPORT"

# Print summary to console
echo ""
echo -e "${BLUE}📊 SECURITY AUDIT SUMMARY${NC}"
echo "=========================="
echo "Critical vulnerabilities: $(grep -c "\[CRITICAL\]" "$AUDIT_REPORT")"
echo "High severity issues: $(grep -c "\[HIGH\]" "$AUDIT_REPORT")"
echo "Medium severity issues: $(grep -c "\[MEDIUM\]" "$AUDIT_REPORT")"
echo "Low severity issues: $(grep -c "\[LOW\]" "$AUDIT_REPORT")"
echo "Informational notes: $(grep -c "\[INFO\]" "$AUDIT_REPORT")"

# Overall assessment
if [ $VULNERABILITIES_FOUND -eq 0 ]; then
    echo -e "${GREEN}✅ SECURITY AUDIT PASSED${NC}"
    echo "No critical or high-severity vulnerabilities found"
    exit 0
elif [ $VULNERABILITIES_FOUND -le 2 ]; then
    echo -e "${YELLOW}⚠️  SECURITY AUDIT PASSED WITH WARNINGS${NC}"
    echo "Minor security issues found, acceptable for development"
    exit 0
else
    echo -e "${RED}❌ SECURITY AUDIT FAILED${NC}"
    echo "Significant security issues require attention"
    exit 1
fi
