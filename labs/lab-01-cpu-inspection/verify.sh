#!/usr/bin/env bash
set -euo pipefail

echo "[+] Compiling Lab 01 Native Executable..."
gcc -O2 inspect_cpu.c -o inspect_cpu

echo "[+] Running Host Execution Test..."
./inspect_cpu

echo "[+] Testing Multi-Arch Compilation via Docker..."
docker run --rm --platform linux/amd64 gcc:13 gcc -O2 -x c - -o /tmp/amd64_test << 'EOF'
#include <stdio.h>
int main() { printf("Docker Platform Test: AMD64 Success\n"); return 0; }
EOF

docker run --rm --platform linux/arm64 gcc:13 gcc -O2 -x c - -o /tmp/arm64_test << 'EOF'
#include <stdio.h>
int main() { printf("Docker Platform Test: ARM64 Success\n"); return 0; }
EOF

echo "[✔] Lab 01 Verification Complete!"
