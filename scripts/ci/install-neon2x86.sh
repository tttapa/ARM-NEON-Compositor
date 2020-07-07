#!/usr/bin/env bash

# Script to download and install Intel ARM_NEON_2_x86_SSE wrapper

set -ex

version="master" # Release tag on GitHub
prefix="${1:-$HOME/.local}"

[ -e "$prefix/lib/cmake/NEON_2_SSE/NEON_2_SSEConfig.cmake" ] \
 && exit 0

cd /tmp
# Download
[ -d "ARM_NEON_2_x86_SSE" ] \
 || git clone --single-branch --depth=1 --branch "$version" \
        https://github.com/intel/ARM_NEON_2_x86_SSE.git
mkdir -p googletest/build && cd $_
# Configure
cmake .. -DCMAKE_INSTALL_PREFIX="$prefix" -DCMAKE_BUILD_TYPE=Release
# Build
make -j$(nproc)
# Install
make install
