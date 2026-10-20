#!/usr/bin/env bash
# Wipe the external flash: overwrite every 64 KiB sector of 0x90000000-0x90800000
# with 0xFF (erased state). Prints the result of each sector, writes no file.
#   tools/wipe.sh                  DFU_PY=path/to/dfu.py tools/wipe.sh  to change the tool
set -uo pipefail
cd "$(dirname "$0")/.."

DFU_PY="${DFU_PY:-tools/dfu.py}"
START=$((0x90000000))
END=$((0x90800000))
STEP=$((0x10000))

[ -f "$DFU_PY" ] || { echo "error: $DFU_PY not found (set DFU_PY)" >&2; exit 1; }

# Preflight: shows the real error (missing pyusb, no device, permissions...)
# instead of hiding it behind 128 identical FAIL.
if ! out=$(python3 "$DFU_PY" --list 2>&1) || [[ "$out" != *"Memory Layout"* ]]; then
  echo "error: no usable DFU device:" >&2
  echo "$out" | tail -5 >&2
  exit 1
fi
echo "$out"
echo

# dfu.py needs a file: a temporary one, removed on exit
tmp=$(mktemp)
trap 'rm -f "$tmp"' EXIT
head -c "$STEP" /dev/zero | tr '\0' '\377' > "$tmp"

fail=0
shown=0
for ((a = START; a < END; a += STEP)); do
  adr=$(printf "0x%08X" "$a")
  if out=$(python3 "$DFU_PY" -D "$tmp" -s "$adr" 2>&1) && [[ "$out" != *"Missing address"* ]]; then
    echo "$adr OK"
  else
    echo "$adr FAIL"
    fail=1
    # show the cause of the first failure only
    if [ $shown -eq 0 ]; then
      echo "$out" | tail -3 | sed 's/^/    /'
      shown=1
    fi
  fi
done

exit $fail