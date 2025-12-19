# ALETHEIA Developer Guide

## 🛠️ Building ALETHEIA

### Prerequisites
- Make build system (optional)
- Python 3.6+ (for AI components - optional)
- QEMU (for multi-target testing - optional)

### Quick Build
```bash
git clone https://github.com/iyotee/Aletheia.git
cd aletheia
make bootstrap  # Builds entire 5-stage chain autonomously
```

### Component Build
```bash
# Build individual stages autonomously
make stage0           # Hex loader (mathematical foundations)
make stage1           # MesCC-ALE (minimal C compiler)
make stage2           # TinyCC-ALE (extended C compiler)
make stage3           # ALETHEIA-Core (GCC-compatible compiler)
make stage4           # ALETHEIA-Full (AI compiler)
make verify           # Verify 100% independence
```

## 🏗️ Architecture Overview

### Bootstrap Chain
```
Hex Loader (Stage 0) - Mathematical foundations
    ↓ builds
MesCC-ALE (Stage 1) - Minimal C compiler
    ↓ builds
TinyCC-ALE (Stage 2) - Extended C compiler
    ↓ builds
ALETHEIA-Core (Stage 3) - GCC-compatible compiler
    ↓ builds
ALETHEIA-Full (Stage 4) - AI-powered production compiler
```

### Multi-Target Architecture
```
ALETHEIA-Full
├── x86-64 Backend
├── ARM64 Backend
└── RISC-V Backend
```

## 🔧 Adding New Features

### 1. Language Features
```c
// In src/aletheia-full/aletheia-full.c
// Add AST node type
typedef enum {
    // ... existing types
    AST_NEW_FEATURE,
} ASTType;

// Add parsing logic
ASTNode* parse_new_feature() {
    // Implementation
}

// Add code generation
void generate_new_feature(ASTNode* node) {
    // Implementation
}
```

### 2. Backend Extensions
```c
// In src/backends/arm64/arm64_backend.c
static void arm64_generate_new_instruction(FILE* out, const char* dest, const char* src) {
    emit_instruction(out, "new_instruction %s, %s", dest, src);
}

// Update TargetBackend structure
backend->generate_new_instruction = arm64_generate_new_instruction;
```

### 3. AI Optimizations
```python
# In ai/training/code_optimizer_model.py
def add_new_optimization_pattern(self, pattern, optimization):
    """Add new AI optimization pattern"""
    # Implementation
```

## 🧪 Testing

### Running Tests
```bash
# Run all tests
make test

# Run specific test categories
make test-compilation
make test-multi-target
make test-ai

# Run benchmarks
cd benchmarks && python run_simple_benchmarks.py
```

### Adding Tests
```bash
# Add to ci/build_test.sh
echo "Testing new feature..."
if ./test_new_feature; then
    log_result "New feature test" "PASS"
else
    log_result "New feature test" "FAIL"
fi
```

## 🔒 Security Considerations

### Bootstrap Security
- Complete mathematical verification from hex code
- Ken Thompson Trust Problem definitively solved
- Zero external dependencies - pure self-bootstrapping

### AI Security
- AI optimizations are conservative by default
- Confidence thresholds prevent unsafe transformations
- Manual override available for security-critical code

## 📈 Performance Optimization

### Compiler Performance
- Multi-threaded compilation support
- Incremental compilation
- Memory-efficient AST processing

### Generated Code Performance
- AI-driven optimization selection
- Architecture-specific tuning
- Profile-guided optimization support

## 🚀 Release Process

### Version Numbering
- Major.Minor.Patch (Semantic Versioning)
- Major: Breaking changes
- Minor: New features
- Patch: Bug fixes

### Release Checklist
- [ ] All tests pass
- [ ] Security audit completed
- [ ] Documentation updated
- [ ] Performance benchmarks run
- [ ] Multi-target validation
- [ ] Bootstrap chain verified

### Release Commands
```bash
# Create release
make release

# Upload to package repositories
make upload-debian
make upload-homebrew
```

## 🤝 Contributing

### Code Style
- C: Follow Linux kernel style
- Python: Follow PEP 8
- Documentation: Clear and comprehensive

### Commit Messages
```
type(scope): description

Types: feat, fix, docs, style, refactor, test, chore
Examples:
feat(arm64): add NEON SIMD support
fix(parser): resolve memory leak in AST
docs(readme): update installation instructions
```

### Pull Request Process
1. Fork the repository
2. Create feature branch
3. Make changes with tests
4. Run full test suite
5. Submit PR with description

## 📚 API Reference

### Core Functions
```c
// Main compilation function
int compile_gcc100(ALETHEIAFullCompiler* compiler, const char* input);

// Multi-target compilation
int compile_for_target(const char* input, const char* output, TargetArch target);

// AI optimization interface
AIAnalysisResult* ai_analyze_code(const char* code, const char* function_name);
char* ai_apply_optimizations(const char* code, AIAnalysisResult* analysis);
```

### Backend Interface
```c
// Target backend operations
typedef struct TargetBackend {
    TargetArch arch;
    const char* name;
    void (*generate_prologue)(FILE* out, int stack_size);
    void (*generate_mov)(FILE* out, const char* dest, const char* src);
    // ... more functions
} TargetBackend;
```

## 🐛 Troubleshooting

### Common Issues

#### Compilation Errors
```bash
# Check build dependencies
make check-deps

# Clean and rebuild
make clean && make all

# Verbose output
make V=1
```

#### Test Failures
```bash
# Run specific failing test
make test-compilation

# Debug test output
./ci/build_test.sh 2>&1 | tee test_debug.log
```

#### Performance Issues
```bash
# Run performance profiler
make profile

# Compare with baseline
./benchmarks/run_simple_benchmarks.py
```

### Getting Help
- GitHub Issues: Bug reports and feature requests
- Documentation: Comprehensive guides in `docs/`
- Community: GitHub Discussions for questions

## 📋 Roadmap

### Short Term (Next Release)
- [ ] Enhanced error messages
- [ ] Debug symbol generation
- [ ] Link-time optimization

### Medium Term (6-12 months)
- [ ] LLVM IR backend
- [ ] WebAssembly support
- [ ] IDE integration plugins

### Long Term (1-2 years)
- [ ] Machine learning optimization
- [ ] Quantum computing support
- [ ] Advanced static analysis

---

*This guide is continuously updated. Check the latest version in the repository.*
