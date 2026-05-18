#!/bin/bash
# rebuild_release_max.sh
# Fully optimized clean rebuild (Release + LTO if enabled in CMakeLists)

set -euo pipefail

BUILD_DIR="build-release"

echo "[1/3] Removing old build dir: ${BUILD_DIR}"
rm -rf "${BUILD_DIR}"

echo "[2/3] Configuring (Release)..."
cmake -S . -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Release

echo "[3/3] Building (all targets)..."
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo "✅ Done. Binaries are in: ${BUILD_DIR}/"
