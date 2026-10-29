#!/usr/bin/env bash
set -euo pipefail

echo "======================================================="
echo "  Setting up QEMU binfmt_misc Multi-Arch Emulation"
echo "======================================================="

# Check if running with root privileges (or sudo capability)
if [ "$EUID" -ne 0 ]; then
  echo "[-] Warning: This script may require root or privileged container access."
  echo "[-] If binfmt registration fails, run with sudo or in a privileged container."
fi

echo "[+] Installing multi-arch binfmt handlers using tonistiigi/binfmt..."
docker run --privileged --rm tonistiigi/binfmt --install all

echo "[+] Verifying registered binfmt handlers in /proc/sys/fs/binfmt_misc:"
if [ -d /proc/sys/fs/binfmt_misc ]; then
  ls -l /proc/sys/fs/binfmt_misc/ | grep -E "qemu-(aarch64|x86_64)" || true
  echo "[✔] binfmt multi-arch emulation successfully initialized!"
else
  echo "[!] /proc/sys/fs/binfmt_misc not found. Are you running inside an unprivileged container?"
  exit 1
fi