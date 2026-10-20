#!/usr/bin/env bash
# Dump the external flash (0x90000000, 8 MiB) to output/device/dump/external_flash_<time>.bin
#   tools/dump.sh [OUT.bin]
set -euo pipefail
cd "$(dirname "$0")/.."

out="${1:-output/device/dump/external_flash_$(date +%Y%m%d_%H%M%S).bin}"

mkdir -p "$(dirname "$out")"
dfu-util -i 0 -a 0 -s 0x90000000:0x800000:force -U "$out"
echo "dump: $out"