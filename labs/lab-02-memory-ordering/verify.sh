#!/usr/bin/env bash
set -euo pipefail

echo "======================================================="
echo "   Verifying Lab 02: Memory Ordering & Concurrency     "
echo "======================================================="

# Navigate to lab directory if not already there
cd "$(dirname "$0")"

echo "[+] Compiling main.cpp with C++17 and optimizations..."
g++ -std=c++17 -O3 main.cpp -pthread -o memory_ordering

echo "[+] Executing memory ordering binary..."
./memory_ordering

echo "[✔] Lab 02 verification completed successfully!"
