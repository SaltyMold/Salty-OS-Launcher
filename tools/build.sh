#!/usr/bin/env bash
# Build the base app and generate output/device/Salty-OS-Launcher-<model>.nwa.
#   tools/build.sh MODEL            MODEL: n0110, n0115, n0120 (or 110, 115, 120)
set -euo pipefail
cd "$(dirname "$0")/.."

model="${1:?usage: $0 MODEL}"
model="n0${model#n0}"

make build PLATFORM=device
mkdir -p base
cp output/device/app.nwa base/app.nwa
tools/link.sh "$model"