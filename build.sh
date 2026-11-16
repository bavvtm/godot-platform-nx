#!/bin/bash

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE}")" && pwd)"

source "${ROOT_DIR}/config"

BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
GODOT_OVERLAY_DIR="${GODOT_OVERLAY_DIR:-${ROOT_DIR}/platform-overlay}"
GODOT_SOURCE="${GODOT_SOURCE:-${BUILD_DIR}/godot}"
GODOT_TARGET="${GODOT_TARGET:-template_debug}"
MESA_OVERLAY_DIR="${MESA_OVERLAY_DIR:-${ROOT_DIR}/mesa}"
MESA_SOURCE="${MESA_SOURCE:-${BUILD_DIR}/mesa}"
MESA_BUILD_TYPE="${MESA_BUILD_TYPE:-debug}"
JOBS=${JOBS:-$(nproc)}

function clone_mesa() {
  if [[ ! -d "${MESA_SOURCE}/.git" ]]; then
    git init "${MESA_SOURCE}" || return 1
    git -C "${MESA_SOURCE}" remote add origin "${MESA_REPO}" || return 1
    git -C "${MESA_SOURCE}" fetch --depth 1 origin "${MESA_REF}" || return 1
    git -C "${MESA_SOURCE}" checkout FETCH_HEAD || return 1
  fi

  if [[ -d "${MESA_OVERLAY_DIR}" ]]; then
    cp -f "${MESA_OVERLAY_DIR}/"* "${MESA_SOURCE}/"
    chmod +x "${MESA_SOURCE}/"*.sh 2>/dev/null || true
  fi

  return 0
}

function build_mesa() {
  if [[ -d "${MESA_SOURCE}/mesa-unified-install" ]]; then
    return 0
  fi

  (
    cd "${MESA_SOURCE}" || exit 1
    MESA_BUILD_TYPE="${MESA_BUILD_TYPE}" ./build-unified-linux.sh
  ) || return 1

  return 0
}

function clone_godot() {
  if [[ ! -d "${GODOT_SOURCE}/.git" ]]; then
    git clone --depth 1 --branch "${GODOT_REF}" "${GODOT_REPO}" "${GODOT_SOURCE}" || return 1
  fi

  rm -rf "${GODOT_SOURCE}/platform/switch"
  ln -sfn "${GODOT_OVERLAY_DIR}" "${GODOT_SOURCE}/platform/switch"

  return 0
}

function build_godot() {
  local obj_dir="${GODOT_SOURCE}/build/obj"
  local mesa_sdk="${MESA_SOURCE}/mesa-unified-install/opt/devkitpro/portlibs/switch"
  local debug_symbols="yes"

  if [[ "${GODOT_TARGET}" == "template_release" ]]; then
    debug_symbols="no"
  fi

  python3 -m SCons -C "${GODOT_SOURCE}" \
    platform=switch \
    target="${GODOT_TARGET}" \
    arch=arm64 \
    vulkan=yes \
    opengl3=yes \
    audren=yes \
    use_lto=no \
    debug_symbols="${debug_symbols}" \
    mesa_sdk="${mesa_sdk}" \
    object_dir="${obj_dir}" \
    -j"${JOBS}" || return 1

  aarch64-none-elf-strip --strip-all \
    "${GODOT_SOURCE}/bin/godot.switch.${GODOT_TARGET}.arm64" || return 1

  cp -f "${GODOT_SOURCE}/bin/godot.switch.${GODOT_TARGET}.arm64" \
    "${BUILD_DIR}/${GODOT_TARGET}"

  cp -f "${GODOT_SOURCE}/bin/godot.switch.${GODOT_TARGET}.arm64.nro" \
    "${BUILD_DIR}/${GODOT_TARGET}.nro"

  return 0
}

mkdir -p "${BUILD_DIR}"

case "${1:-all}" in
  mesa)
    clone_mesa || exit 1
    build_mesa || exit 1
    ;;
  godot)
    clone_godot || exit 1
    build_godot || exit 1
    ;;
  all|*)
    clone_mesa || exit 1
    clone_godot || exit 1
    build_mesa || exit 1
    build_godot || exit 1
    ;;
esac
