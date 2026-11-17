#!/bin/bash
set -e

if [ "$(id -u)" -ne 0 ]; then
  SUDO="sudo"
else
  SUDO=""
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE}")" && pwd)"

source "${ROOT_DIR}/config"

$SUDO apt-get update
$SUDO apt-get update
$SUDO apt-get install -y --no-install-recommends \
  build-essential cmake ninja-build pkg-config \
  flex bison curl zip git ca-certificates \
  python3 python3-pip python3-pil python3-setuptools python3-wheel \
  clang libclang-dev llvm-dev \
  spirv-tools spirv-headers mingw-w64

LLVM_VER="$(llvm-config --version | cut -d. -f1)"
$SUDO apt-get install -y --no-install-recommends \
  "libclc-${LLVM_VER}-dev" \
  "libclc-${LLVM_VER}" \
  "libllvmspirvlib-${LLVM_VER}-dev"

$SUDO apt-get clean
$SUDO rm -rf /var/lib/apt/lists/*

pip3 install --user --break-system-packages meson mako scons pillow

# install rust ---

if ! command -v rustup >/dev/null 2>&1; then
  curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain nightly
  # shellcheck disable=SC1091
  source "${HOME}/.cargo/env"
fi
rustup target add aarch64-unknown-linux-gnu
rustup component add rust-src
command -v bindgen >/dev/null 2>&1 || cargo install bindgen-cli
command -v cbindgen >/dev/null 2>&1 || cargo install cbindgen

sudo dkp-pacman -Syu --noconfirm \
  switch-dev \
  switch-pkg-config \
  switch-zlib \
  switch-freetype \
  switch-bzip2 \
  switch-mbedtls \
  switch-libpng \
  switch-libwebp \
  switch-libogg \
  switch-libvorbis \
  switch-libzstd \
  switch-libexpat

echo "Setup completed successfully!"
