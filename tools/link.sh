#!/usr/bin/env bash
# Patch base/app.nwa with the userland of base/userland.onboarding.allow3rdparty.B.<model>.dfu.
#   tools/link.sh MODEL [OUT.nwa]   MODEL: n0110, n0115, n0120 (or 110, 115, 120)
#   default OUT: output/device/Salty-OS-Launcher-<model>.nwa
set -euo pipefail
cd "$(dirname "$0")/.."

model="${1:?usage: $0 MODEL [OUT.nwa]}"
model="n0${model#n0}"   # 120 / n0120 -> n0120
out="${2:-output/device/Salty-OS-Launcher-$model.nwa}"
dfu="base/userland.onboarding.allow3rdparty.B.$model.dfu"

[ -f base/app.nwa ] || { echo "error: base/app.nwa missing (run tools/build.sh first)" >&2; exit 1; }
[ -f "$dfu" ] || { echo "error: $dfu missing" >&2; exit 1; }

mkdir -p "$(dirname "$out")"
python3 scripts/patch_dfu.py base/app.nwa "$dfu" "${model#n0}" "$out"