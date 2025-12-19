#!/bin/bash

# ALETHEIA Release Creation Script
# Creates packages for all platforms and prepares GitHub release

set -e

VERSION=${1:-"1.0.0"}
RELEASE_DIR="release_$VERSION"

echo "=== Creating ALETHEIA Release v$VERSION ==="

# Clean previous builds
echo "Cleaning previous builds..."
make clean
rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"

# Build compiler first
echo "Building compiler..."
make all

# Build Windows Chocolatey package (if on Windows with PowerShell)
echo "Building Windows Chocolatey package..."
if command -v powershell.exe >/dev/null 2>&1; then
    cd distribution/scripts
    if powershell.exe -ExecutionPolicy Bypass -File build_chocolatey.ps1 -Version "$VERSION"; then
        cp choco_build/aletheia.$VERSION.nupkg "../../$RELEASE_DIR/" 2>/dev/null && echo "✅ Chocolatey package copied"
    else
        echo "❌ Chocolatey build failed"
    fi
    cd ../..
else
    echo "PowerShell not available, skipping Chocolatey build"
fi

# Build Debian package (Linux only)
if command -v dpkg-buildpackage >/dev/null 2>&1; then
    echo "Building Debian package..."
    cd distribution/scripts
    if ./build_debian.sh; then
        cp ../debian_build/aletheia_$VERSION_amd64.deb "../../$RELEASE_DIR/" 2>/dev/null && echo "✅ Debian package copied"
    else
        echo "❌ Debian build failed"
    fi
    cd ../..
else
    echo "Skipping Debian build (not on Linux or dpkg-buildpackage not available)"
fi

# Build RPM package (Linux only)
if command -v rpmbuild >/dev/null 2>&1; then
    echo "Building RPM package..."
    cd distribution/scripts
    if ./build_rpm.sh; then
        cp packages/*.rpm "../../$RELEASE_DIR/" 2>/dev/null && echo "✅ RPM package copied"
    else
        echo "❌ RPM build failed"
    fi
    cd ../..
else
    echo "Skipping RPM build (not on Linux or rpmbuild not available)"
fi

echo ""
echo "=== Release v$VERSION Created ==="
echo "Files in $RELEASE_DIR/:"
ls -la "$RELEASE_DIR/"

echo ""
echo "=== Next Steps ==="
echo "1. Test all packages locally"
echo "2. Create GitHub release at: https://github.com/iyotee/Aletheia/releases/new"
echo "3. Upload all files from $RELEASE_DIR/"
echo "4. Add release notes"
echo "5. Publish!"
echo ""
echo "For official repositories:"
echo "- Chocolatey: Submit .nupkg to chocolatey.org"
echo "- Debian: Create PPA on launchpad.net"
echo "- RPM: Submit to Fedora/CentOS repos"
