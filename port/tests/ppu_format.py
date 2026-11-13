#!/usr/bin/env python3
"""Exercise capture compatibility and malformed-input handling without a ROM."""
import gzip
from pathlib import Path
import struct
import subprocess
from ppu_regression import BUILD, build, run


def main():
    build(sanitize=True)
    directory = BUILD / 'format'
    directory.mkdir(parents=True, exist_ok=True)
    banks = [bytearray(n) for n in (0x400, 0x400, 0x18000, 0x400)]
    origins = bytes(16)
    legacy, delta = bytearray(b'PPUDUMP1'), bytearray(b'PPUDUMP2')
    expected = bytearray(b'P6\n240 160\n255\n')
    for y in range(160):
        channels = (y % 32, (y // 2) % 32, (159 - y) % 32)
        struct.pack_into('<H', banks[1], 0, channels[0] | (channels[1] << 5) | (channels[2] << 10))
        expected.extend(bytes((c << 3) | (c >> 2) for c in channels) * 240)
        legacy.extend(origins + b''.join(banks))
        delta.extend(origins + bytes([15 if y == 0 else 2]))
        delta.extend(b''.join(banks) if y == 0 else banks[1])
    for name, dump in [('legacy', legacy), ('delta', delta)]:
        result = subprocess.run([str(BUILD / 'ppu_render')], input=dump, capture_output=True, check=True)
        if result.stdout != expected:
            raise AssertionError(f'{name}: reconstructed pixels differ')
        (directory / (name + '.ppm')).write_bytes(expected)
        with gzip.open(directory / (name + '.ppu.gz'), 'wb') as out:
            out.write(dump)
    no_banks, illegal_mask = bytearray(delta), bytearray(delta)
    no_banks[24], illegal_mask[24] = 0, 16
    invalid = {
        'empty': b'', 'bad_magic': b'INVALID!', 'short_origins': delta[:23],
        'missing_initial_banks': no_banks, 'invalid_mask_bits': illegal_mask,
        'short_bank': delta[:100], 'short_last_line': delta[:-1],
        'trailing_data': delta + b'!', 'short_legacy': legacy[:-1],
    }
    for name, dump in invalid.items():
        result = subprocess.run([str(BUILD / 'ppu_render')], input=dump, capture_output=True)
        if result.returncode != 1:
            raise AssertionError(f'{name}: expected clean rejection, got {result.returncode}')
    run(['node', 'port/tests/ppu_demo.mjs', directory])
    run(['node', 'port/tests/ppu_wasm.mjs', BUILD / 'ppu.wasm', directory])
    print(f'Capture formats: both versions exact; {len(invalid)} malformed inputs rejected under ASan/UBSan')


if __name__ == '__main__':
    main()
