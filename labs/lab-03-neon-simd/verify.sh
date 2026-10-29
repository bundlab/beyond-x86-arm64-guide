#!/usr/bin/env bash
set -euo pipefail

echo "======================================================="
echo "   Verifying Lab 03: ARM64 NEON SIMD Benchmark         "
echo "======================================================="

cd "$(dirname "$0")"

ARCH=$(uname -m)

if [ "$ARCH" = "aarch64" ] || [ "$ARCH" = "arm64" ]; then
    echo "[+] Detected native ARM64 host. Compiling and running locally..."
    make clean && make
    ./neon_benchmark
else
    echo "[+] Detected x86_64 host. Utilizing Docker with ARM64 emulation..."
    docker run --rm --platform linux/arm64 -v "$(pwd)":/work -w /work gcc:13-bookworm bash -c "
        apt-get update && apt-get install -y make
        make clean && make
        ./neon_benchmark
    "
fi

echo "[✔] Lab 03 verification completed successfully!"