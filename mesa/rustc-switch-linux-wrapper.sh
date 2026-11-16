#!/bin/bash
set -e

MESA="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

for arg in "$@"; do
  case "$arg" in
    */meson-private/sanity.rs)
      ARGS=()
      skip=0
      for a in "$@"; do
        if [ "$skip" -eq 1 ]; then
          skip=0
          case "$a" in
            linker=*) continue ;;
          esac
          ARGS+=("-C" "$a")
          continue
        fi
        case "$a" in
          -Clinker=*) continue ;;
          -C) skip=1 ;;
          *) ARGS+=("$a") ;;
        esac
      done
      exec "${MESA_SWITCH_RUSTC:-rustc}" "${ARGS[@]}"
      ;;
  esac
done

export MESA_SWITCH_RUST_TARGET="${MESA_SWITCH_RUST_TARGET:-aarch64-unknown-linux-gnu}"
exec /bin/bash "$MESA/rustc-switch-wrapper.sh" "$@"
