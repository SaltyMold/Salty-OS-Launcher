#!/usr/bin/env python3
"""Fill the empty DFU slot of a base .nwa with the userland of a .dfu.

  patch_dfu.py BASE.nwa FILE.dfu MODEL OUT.nwa      (MODEL: 110, 115 or 120)

BASE.nwa is the app built once, with an empty slot (src/storage/dfu_slot_data.c).
It is copied to OUT.nwa, which gets the userland. Same size as BASE.nwa.

The slot is: marker 16 | capacity 4 | model 4 | size 4 | data. The data is the
elements of the DfuSe file as records (address u32, length u32, bytes), so the
app never parses DfuSe.
"""
import struct
import sys
import zlib

MAGIC = b"SALTYDFU_SLOT2\0\0"
HEADER = 28
SLOT_B = (0x90400000, 0x90800000)
MODELS = (110, 115, 120)


def fail(message):
    sys.exit(f"error: {message}")


def dfuse_elements(path):
    """The (address, bytes) elements of a DfuSe file."""
    blob = open(path, "rb").read()
    if blob[:5] != b"DfuSe" or len(blob) < 27:
        fail(f"{path}: not a DfuSe file")
    image_size, targets = struct.unpack_from("<IB", blob, 6)
    if image_size + 16 != len(blob):
        fail(f"{path}: size field does not match the file")
    if blob[image_size + 8 : image_size + 11] != b"UFD":
        fail(f"{path}: bad DFU suffix")
    if struct.unpack_from("<I", blob, len(blob) - 4)[0] != ~zlib.crc32(blob[:-4]) & 0xFFFFFFFF:
        print(f"warning: {path}: CRC does not match", file=sys.stderr)

    elements, pos = [], 11
    for _ in range(targets):
        if blob[pos : pos + 6] != b"Target":
            fail(f"{path}: target not found at {pos}")
        size, count = struct.unpack_from("<II", blob, pos + 266)
        pos += 274
        end = pos + size
        for _ in range(count):
            address, length = struct.unpack_from("<II", blob, pos)
            pos += 8
            if pos + length > end:
                fail(f"{path}: element runs past its target")
            elements.append((address, blob[pos : pos + length]))
            pos += length
        if pos != end:
            fail(f"{path}: target size does not match its elements")
    if not elements or pos != image_size:
        fail(f"{path}: no elements, or trailing bytes")
    return elements


def records(path):
    out = b""
    for address, data in dfuse_elements(path):
        data += b"\xff" * (-len(data) % 4)  # 0xFF changes nothing in erased flash
        if address % 4 or address < SLOT_B[0] or address + len(data) > SLOT_B[1]:
            fail(f"{path}: element {address:#x}+{len(data):#x} is outside slot B")
        out += struct.pack("<II", address, len(data)) + data
    return out


def patch(base_path, dfu_path, model, out_path):
    if model not in MODELS:
        fail(f"model must be one of {MODELS}")
    nwa = bytearray(open(base_path, "rb").read())
    data = records(dfu_path)

    at = nwa.find(MAGIC)
    if at < 0 or nwa.find(MAGIC, at + 1) >= 0:
        fail("slot marker not found exactly once in the base .nwa")
    capacity = struct.unpack_from("<I", nwa, at + 16)[0]
    start = at + HEADER
    if len(data) > capacity:
        fail(f"userland needs {len(data)} bytes, the slot holds {capacity}: rebuild the base")
    if nwa[start : start + capacity] != b"\xff" * capacity:
        fail("the base slot is not empty")

    struct.pack_into("<II", nwa, at + 20, model, len(data))
    nwa[start : start + len(data)] = data
    open(out_path, "wb").write(nwa)
    print(f"{out_path}: n0{model}, {len(data)} / {capacity} bytes")


if __name__ == "__main__":
    a = sys.argv[1:]
    if len(a) != 4:
        sys.exit(__doc__)
    patch(a[0], a[1], int(a[2]), a[3])