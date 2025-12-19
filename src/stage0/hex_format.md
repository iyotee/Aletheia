# ALETHEIA Hex Bootstrap Format Specification

## Overview

The Hex Bootstrap Format defines how C source code is encoded for loading by the pure machine code hex loader. This format allows the Stage 0 hex loader to parse and execute C source code without any external tools.

## Format Structure

### Basic Encoding
- **Input**: ASCII text C source code
- **Output**: Hex-encoded byte stream
- **Encoding**: Each source byte → 2 hex characters (0-9, A-F)
- **Format**: Space-separated hex byte pairs

### Example
```
C source: int main() { return 42; }
Hex encoded: 69 6E 74 20 6D 61 69 6E 28 29 20 7B 20 72 65 74 75 72 6E 20 34 32 3B 20 7D
```

## File Structure

### Header Section
```
# ALETHEIA_BOOTSTRAP v1.0
# File: filename.c
# Size: XXXX bytes
# Checksum: XXXX
```

### Data Section
```
# Begin data
XX XX XX XX ... # Hex encoded source bytes
# End data
```

### Footer Section
```
# EOF
```

## Complete Example

```
# ALETHEIA_BOOTSTRAP v1.0
# File: simple.c
# Size: 23 bytes
# Checksum: 1234
# Begin data
69 6E 74 20 6D 61 69 6E 28 29 20 7B 0A 20 20 72 65 74 75 72 6E 20 34 32 3B 0A 7D
# End data
# EOF
```

## Parsing Rules

### Character Processing
1. **Skip whitespace**: Spaces, tabs, newlines, carriage returns
2. **Skip comments**: Lines starting with `#` are ignored
3. **Hex pairs**: Every 2 hex characters → 1 byte
4. **Case insensitive**: A-F and a-f both accepted

### Error Handling
- Invalid hex characters → Error condition
- Odd number of hex digits → Error condition
- Unexpected EOF → Graceful termination

## Conversion Tools

### Encoding Tool (Temporary - will be removed)
```bash
# Convert C file to hex format
hexencode() {
    echo "# ALETHEIA_BOOTSTRAP v1.0"
    echo "# File: $1"
    echo "# Size: $(stat -c%s "$1") bytes"
    echo "# Begin data"
    hexdump -v -e '1/1 " %02X"' "$1" | fold -w 48
    echo ""
    echo "# End data"
    echo "# EOF"
}
```

### Decoding Tool (For verification)
```bash
# Convert hex format back to binary
hexdecode() {
    grep -v '^#' "$1" | tr -d ' ' | xxd -r -p
}
```

## Stage 0 Integration

### Loader Requirements
The Stage 0 hex loader must:
1. Read hex-encoded input from stdin
2. Parse according to this specification
3. Convert hex pairs to binary bytes
4. Output executable machine code

### Bootstrap Process
1. **Stage 0**: Pure machine code hex loader
2. **Input**: MesCC-ALE source in hex format
3. **Output**: MesCC-ALE compiler binary
4. **Result**: First compiler built from pure machine code

## Security Considerations

### Trust Anchors
- Hex format parsing must be mathematically verified
- No external library dependencies
- Deterministic conversion process

### Verification
- Checksum validation for corruption detection
- Size verification for completeness
- Format validation for correctness

## Future Extensions

### Compression
- Add simple run-length encoding
- Implement basic LZ compression
- Maintain bootstrap simplicity

### Metadata
- Add symbol table information
- Include debugging information
- Support for multiple files

### Optimization
- Strip unnecessary whitespace
- Remove comments automatically
- Minify source during encoding

## Implementation Status

### Completed ✅
- [x] Basic hex encoding specification
- [x] File format structure
- [x] Parsing rules definition
- [x] Error handling guidelines
- [x] Example implementations

### In Progress 🚧
- [ ] MesCC-ALE source conversion to hex format
- [ ] Stage 0 hex loader integration
- [ ] Bootstrap verification testing

### Planned 📋
- [ ] Compression algorithms
- [ ] Multi-file support
- [ ] Symbol table encoding

