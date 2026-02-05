#!/bin/bash
# rebuild_release_max.sh
set -euo pipefail

# Clean build
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build (use all cores)
cmake --build build -j"$(nproc)"
