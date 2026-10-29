#!/usr/bin/env bash
set -euo pipefail

BUILDER_NAME="multiarch-builder"

echo "======================================================="
echo "  Setting up Docker Buildx Multi-Arch Builder"
echo "======================================================="

# Check if Docker is running
if ! docker info > /dev/null 2>&1; then
  echo "[✘] Error: Docker daemon is not running or accessible."
  exit 1
fi

# Check if buildx is available
if ! docker buildx version > /dev/null 2>&1; then
  echo "[✘] Error: Docker Buildx is not installed or enabled."
  exit 1
fi

# Remove builder if it already exists to ensure a clean state
if docker buildx inspect "$BUILDER_NAME" > /dev/null 2>&1; then
  echo "[+] Removing existing builder instance: $BUILDER_NAME"
  docker buildx rm "$BUILDER_NAME"
fi

echo "[+] Creating new Docker Buildx instance: $BUILDER_NAME"
docker buildx create \
  --name "$BUILDER_NAME" \
  --driver docker-container \
  --bootstrap \
  --use

echo "[+] Inspecting active builder capabilities..."
docker buildx inspect --bootstrap

echo "[✔] Buildx builder '$BUILDER_NAME' is ready for multi-arch compilation!"