#!/usr/bin/env python3
"""Deterministic GBA display fixtures compared with mGBA, without a game ROM."""
import argparse
import copy
import json
from pathlib import Path
import random
import shlex
import struct
import subprocess
from ppu_regression import ROOT, BUILD, run, build_ppu


def w16(data, offset, value):
    struct.pack_into('<H', data, offset, value & 65535)


def w32(data, offset, value):
    struct.pack_into('<I', data, offset, value & 0xffffffff)


class Scene:
    def __init__(self, seed=123):
        rng = random.Random(seed)
        self.io = bytearray(0x400)
        self.pal = bytearray(rng.randbytes(0x400))
        self.vram = bytearray(rng.randbytes(0x18000))
        self.oam = bytearray(0x400)
        self.events = []
        for i in range(128):
            w16(self.oam, i * 8, 0x200)
        for bg in range(4):
            # Four independent tilemaps in the upper half of BG VRAM.
            w16(self.io, 8 + bg * 2, ((16 + bg * 4) << 8) | bg)
            for i in range(4096):
                w16(self.vram, 0x8000 + bg * 0x2000 + i * 2, rng.randrange(256) | (rng.randrange(64) << 10))
            w16(self.io, 0x10 + bg * 4, 233 + bg * 47)
            w16(self.io, 0x12 + bg * 4, 179 + bg * 53)
        for bg in range(2):
            w16(self.io, 0x20 + bg * 16, 256)
            w16(self.io, 0x26 + bg * 16, 256)

    def reg(self, offset, value):
        w16(self.io, offset, value)
        return self

    def obj(self, i, a, b, c):
        for j, value in enumerate((a, b, c)):
            w16(self.oam, i * 8 + j * 2, value)
        return self

    def matrix(self, i, a, b, c, d):
        for j, value in enumerate((a, b, c, d)):
            w16(self.oam, i * 32 + 6 + j * 8, value)
        return self

    def write(self, path):
        path.write_bytes(self.io + self.pal + self.vram + self.oam +
                         b''.join(struct.pack('<HHH', *e) for e in sorted(self.events)))


def fixtures():
    yield 'forced_blank', Scene().reg(0, 0x80)
    yield 'backdrop', Scene()
    for bg in range(4):
        for depth in (4, 8):
            for size in range(4):
                s = Scene().reg(0, 0x100 << bg)
                s.reg(8 + bg * 2, 0x8000 * 0 + ((16 + bg * 4) << 8) | (size << 14) | (0x80 if depth == 8 else 0))
                yield f'text_bg{bg}_{depth}bpp_size{size}', s
    s = Scene().reg(0, 0xf00)
    yield 'text_priorities', s
    for effect in range(4):
        for coeff in (0, 7, 16, 31):
            s = Scene().reg(0, 0xf00).reg(0x50, 0x3f3f | (effect << 6)).reg(0x52, coeff | ((31 - coeff) << 8)).reg(0x54, coeff)
            yield f'blend_{effect}_{coeff}', s
    for mode in (1, 2, 3, 4, 5):
        for transform in ('identity', 'rotate', 'negative', 'mosaic'):
            s = Scene().reg(0, mode | (0xc00 if mode == 2 else 0x400))
            for bg in (2, 3):
                s.reg(8 + bg * 2, 0x2000 | (24 << 8) | (2 << 14) | (64 if transform == 'mosaic' else 0))
                if transform != 'identity':
                    s.reg(0x20 + (bg-2)*16, 181).reg(0x22 + (bg-2)*16, -181)
                    s.reg(0x24 + (bg-2)*16, 181).reg(0x26 + (bg-2)*16, 181)
                    w32(s.io, 0x28+(bg-2)*16, -3000 if transform == 'negative' else 16000)
            s.reg(0x4c, 0x65)
            yield f'mode{mode}_{transform}', s
        yield f'mode{mode}_page1', copy.deepcopy(s).reg(0, mode | 0x410)
    for shape in range(3):
        for size in range(4):
            for depth in (4, 8):
                s = Scene().reg(0, 0x1040)
                a = 34 | (shape << 14) | (0x2000 if depth == 8 else 0)
                b = 53 | (size << 14)
                for i, flip in enumerate((0, 0x1000, 0x2000, 0x3000)):
                    s.obj(i, a + (i//2)*63, b + (i%2)*100 | flip, 0x5030)
                yield f'obj_shape{shape}_size{size}_{depth}bpp', s
    for mapping in (0, 64):
        for double in (0, 0x200):
            for depth in (0, 0x2000):
                s = Scene().reg(0, 0x1000 | mapping)
                s.obj(0, 30 | 0x100 | double | depth, 45 | 0xc000, 0x9030)
                s.matrix(0, 181, -181, 181, 181)
                yield f'obj_affine_{mapping}_{double}_{depth}', s
    for x in (0, 1, 237, 240, 255, 256, 480, 511):
        s = Scene().reg(0, 0x1040).obj(0, 240, x | 0xc000, 0x20)
        yield f'obj_clip_{x}', s
    for mosaic in (0x11, 0x65, 0xff):
        yield f'bg_mosaic_{mosaic}', Scene().reg(0, 0x100).reg(8, 0x1040).reg(0x4c, mosaic)
        for affine in (0, 0x100):
            s = Scene().reg(0, 0x1040).reg(0x4c, mosaic << 8)
            s.obj(0, 33 | 0x1000 | affine, 37 | 0x8000, 0x20).matrix(0, 181, -181, 181, 181)
            yield f'obj_mosaic_{mosaic}_{affine}', s
    for wrap in (False, True):
        for masks in (0x3f00, 0x153a, 0x2a05):
            s = Scene().reg(0, 0xff00).reg(0x40, 0xc828 if wrap else 0x28c8).reg(0x42, 0x5090)
            s.reg(0x44, 0x8c14 if wrap else 0x148c).reg(0x46, 0x3278)
            s.reg(0x48, masks).reg(0x4a, masks ^ 0x3f3f)
            s.obj(0, 0x800 | 50, 70 | 0xc000, 0x20)
            yield f'windows_{wrap}_{masks}', s
    for effect in range(4):
        for masks in (0x3f3f, 0x0010, 0x2000, 0x0020):
            s = Scene().reg(0, 0x1f40).reg(0x50, masks | (effect << 6)).reg(0x52, 0x0907).reg(0x54, 9)
            s.obj(0, 0x400 | 30, 40 | 0xc000, 0x20)
            s.obj(1, 40, 50 | 0xc000, 0x420)
            yield f'semiobj_{effect}_{masks}', s
    s = Scene().reg(0, 0x401).reg(12, 0x3800)
    for line in range(160):
        s.events += [(line, 0x28, (line * 256 + 113) & 65535), (line, 0x2c, (line * 512) & 65535)]
    yield 'affine_hblank_reference', s
    s = Scene().reg(0, 0x100)
    for line in range(160):
        s.events += [(line, 0x10, line * 3), (line, 0x50, 0x81), (line, 0x54, line % 17)]
    yield 'text_hblank_scroll_brightness', s
    yield 'greenswap', Scene().reg(0, 0x100).reg(2, 1)


def edge_fixtures():
    for mapping in (0, 64):
        for tile in (1, 31, 63, 511, 1007, 1023):
            for depth in (0, 0x2000):
                s = Scene().reg(0, 0x1000 | mapping)
                s.obj(0, 20 | depth, 40 | 0xc000, tile)
                yield f'obj_tilewrap_{mapping}_{tile}_{depth}', s
    for affine in (0, 0x100):
        for hblank in (0, 32):
            s = Scene().reg(0, 0x1040 | hblank).matrix(31, 256, 0, 0, 256)
            for i in range(112):
                s.obj(i, 20 | affine, (i * 19) % 220 | 0xc000 | (31 << 9 if affine else 0), (i * 16) & 1023)
            yield f'obj_budget_{affine}_{hblank}', s
    for depth in (0, 128):
        s = Scene().reg(0, 0x100).reg(8, 0x1f0c | depth)
        for i in range(1024):
            w16(s.vram, 0xf800 + i * 2, 1023)
        yield f'bg_tile_overflow_{depth}', s
    for mode in (3, 4, 5):
        s = Scene().reg(0, 0x1440 | mode)
        for i in range(4):
            s.obj(i, 20 + i * 32, 40 | 0x8000, (i % 2)*512 | (i << 10))
        yield f'bitmap_obj_overlap_{mode}', s
    for mode in (3, 4, 5):
        for mapping in (0, 64):
            for tile in (511, 512, 1007, 1023):
                for depth in (0, 0x2000):
                    s = Scene().reg(0, 0x1400 | mode | mapping)
                    s.obj(0, 20 | depth, 40 | 0xc000, tile)
                    yield f'bitmap_obj_wrap_{mode}_{mapping}_{tile}_{depth}', s
    for priority in range(4):
        for semi in (0, 0x400):
            s = Scene().reg(0, 0x1f40).reg(0x50, 0x3f7f).reg(0x52, 0x0808)
            s.obj(0, 20 | semi, 30 | 0xc000, 0x20 | (priority << 10))
            s.obj(1, 25, 35 | 0xc000, 0x120 | (priority << 10))
            yield f'obj_priority_{priority}_{semi}', s
    for flags in (0x300, 0x100, 0x200, 0):
        s = Scene().reg(0, 0x100).reg(8, 0x1000).reg(0x50, 0x3f7f).reg(0x52, 0x0808)
        for line in range(160):
            s.events.append((line, 0x10, line * 2))
        for i in range(40):
            s.obj(i, 20 | flags, 30 | 0xc000, i*16).matrix(i%32, 256, 0, 0, 256)
        yield f'obj_disabled_{flags}', s
    for bgmask in (0x100, 0xf00):
        for masks in (0x153a, 0x2a05, 0x3f00):
            for semi in (0, 0x400):
                s = Scene().reg(0, 0xf040 | bgmask).reg(0x40, 0x28c8).reg(0x42, 0x5090)
                s.reg(0x44, 0x148c).reg(0x46, 0x3278).reg(0x48, masks).reg(0x4a, masks ^ 0x3f3f)
                s.reg(0x50, 0x3f7f).reg(0x52, 0x0907)
                s.obj(0, 0x800 | 50, 70 | 0xc000, 0x20)
                s.obj(1, 40 | semi, 40 | 0xc000, 0x820)
                yield f'objwin_blend_{bgmask}_{masks}_{semi}', s


def affine_window_fixtures():
    for bg in (2, 3):
        for size in range(4):
            for wrap in (0, 0x2000):
                s = Scene().reg(0, 2 | (0x100 << bg))
                s.reg(8 + bg * 2, (20 << 8) | (size << 14) | wrap)
                reg = 0x20 + (bg-2)*16
                s.reg(reg, 381).reg(reg+2, -92).reg(reg+4, 172).reg(reg+6, 247)
                w32(s.io, reg+8, -35 * 256)
                w32(s.io, reg+12, 12 * 256)
                yield f'affine_bg{bg}_size{size}_wrap{wrap}', s
    for mapping in (0, 64):
        for x in (0, 1, 230, 511):
            for flip in (0, 0x3000):
                s = Scene().reg(0, 0x1000 | mapping).reg(0x4c, 0x6500)
                s.obj(0, 253 | 0x1000, x | 0x8000 | flip, 31)
                yield f'obj_mosaic_clip_{mapping}_{x}_{flip}', s
    for start, end in ((0, 0), (0, 240), (240, 255), (255, 20), (250, 249), (50, 50)):
        for vertical in (False, True):
            s = Scene().reg(0, 0x2f00).reg(0x48, 0x3f).reg(0x4a, 0)
            s.reg(0x40, 0x00f0 if vertical else (start << 8) | end)
            s.reg(0x44, (start << 8) | end if vertical else 0x00a0)
            yield f'window_bounds_{start}_{end}_{vertical}', s
    for equal in (False, True):
        s = Scene().reg(0, 0x1f40).reg(0x50, 0x3f7f).reg(0x52, 0x0808)
        s.obj(0, 20, 30 | 0xc000, 0x20)
        s.obj(1, 20, 30 | 0xc000, 0x120 | (0 if equal else 0xc00))
        # Transparent holes in a front OBJ must not erase an underlying OBJ.
        for i in range(0x400): s.vram[0x10400+i] = 0 if i%3 else 0x55
        yield f'obj_transparent_priority_{equal}', s


def priority_latch_fixtures():
    for effect in (0, 1, 2, 3):
        for kind in (0, 0x400, 0x800):
            s = Scene().reg(0, 0x9f40).reg(0x50, 0x3f3f | (effect << 6)).reg(0x52, 0x0808).reg(0x54, 9)
            s.reg(0x4a, 0x3f3f)
            s.obj(0, 20, 30 | 0xc000, 0x120 | 0xc00)
            s.obj(1, 20 | kind, 30 | 0xc000, 0x20)
            for i in range(0x400): s.vram[0x10400+i] = 0 if i%3 else 0x55
            yield f'obj_transparent_late_{effect}_{kind}', s
    for win in range(2):
        s = Scene().reg(0, (0x2000 << win) | 0xf00).reg(0x40+win*2, 0x14dc)
        s.reg(0x44+win*2, 0x1450).reg(0x48, 0x3f3f).reg(0x4a, 0)
        s.events = [(50, 0x44+win*2, 0x645a), (120, 0x44+win*2, 0x7896)]
        yield f'window_hblank_vertical_{win}', s


def build_oracle(sanitize=False):
    result = subprocess.check_output(['make', '-Bn', '-C', 'tools/emu', 'harness'], cwd=ROOT, text=True)
    command = shlex.split(next(line for line in result.splitlines() if ' -o harness harness.c ' in line))
    command[command.index('harness.c')] = 'port/tests/ppu_oracle.c'
    command[command.index('-o') + 1] = str(BUILD / 'ppu_oracle')
    command += ['-Iport/include', 'port/ppu/ppu.c']
    if sanitize:
        command += ['-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
    run(command)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--filter', default='')
    ap.add_argument('--sanitize', action='store_true')
    ap.add_argument('--wasm', action='store_true', help='also compare linked WebAssembly with mGBA')
    args = ap.parse_args()
    directory = BUILD / 'synthetic'
    directory.mkdir(parents=True, exist_ok=True)
    build_oracle(args.sanitize)
    if args.wasm:
        build_ppu(BUILD)
    rows = []
    for name, scene in (*fixtures(), *edge_fixtures(), *affine_window_fixtures(), *priority_latch_fixtures()):
        if args.filter not in name:
            continue
        path = directory / (name + '.bin')
        scene.write(path)
        command = [str(BUILD / 'ppu_oracle'), str(path)]
        if args.wasm:
            command.append(str(path.with_suffix('.ppm')))
        result = subprocess.run(command, capture_output=True, text=True)
        if result.returncode not in (0, 1):
            raise RuntimeError(name + '\n' + result.stderr)
        count = int(result.stdout.strip())
        rows.append(dict(scene=name, mismatches=count))
        print(f'{name}: {count} / 38400', flush=True)
        if count:
            print(result.stderr, end='')
    if not rows:
        raise RuntimeError('no synthetic fixtures selected')
    (directory / 'results.json').write_text(json.dumps(rows, indent=2) + '\n')
    print(f'{sum(r["mismatches"] == 0 for r in rows)}/{len(rows)} fixtures exact')
    if args.wasm:
        run(['node', 'port/tests/ppu_synthetic_wasm.mjs', BUILD / 'ppu.wasm', directory])
    raise SystemExit(any(r['mismatches'] for r in rows))


if __name__ == '__main__':
    main()
