"""Build the early boot vertical slice as a real wasm module or host executable."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

from addresses import bind_ir, read_addresses
from prepare import prepare_headers, prepare_source
from mmio import instrument_mmio

GAME = ['src/fn/sub_08248970.c', 'src/fn/sub_08249210.c',
        'src/fn/sub_082284F4.c', 'src/fn/sub_08219DD8.c', 'src/fn/sub_08224EEC.c']
CORE = ['port/core/memory.c', 'port/core/timing.c', 'port/core/boot.c']
EXPORTS = ['port_memory_init', 'port_memory_reset', 'port_load_rom', 'port_rom_size',
           'port_boot_early', 'port_boot_call_count', 'port_boot_copy_iwram',
           'port_frame_count', 'port_timing_reset', 'port_run_frame',
           'SceneLifecycle_InitFrameCounterPointer']


def find_linker():
    explicit = os.environ.get('WASM_LD')
    if explicit:
        path = Path(explicit).expanduser()
        if path.is_file(): return str(path)
        found = shutil.which(explicit)
        if found: return found
        raise RuntimeError('WASM_LD does not name an executable linker')
    fallback = Path.home() / 'git/boktai3-decomp/build/toolchain/wasm-ld'
    if fallback.is_file(): return str(fallback)
    found = shutil.which('wasm-ld')
    if found: return found
    raise RuntimeError('wasm-ld not found; set WASM_LD')


def build_slice(root, args):
    out = root / 'build/port' / ('slice-' + args.target)
    out.mkdir(parents=True, exist_ok=True)
    headers = out / 'include'
    prepare_headers(root, headers, args.target)
    hardware = Path(args.hardware_root).resolve() if args.hardware_root else root / 'port'
    bios = hardware / 'hw/bios.c'
    if not bios.exists():
        raise RuntimeError('BIOS source missing; integrate port-hw or pass --hardware-root PATH/port')
    arch = ['--target=wasm32', '-ffreestanding'] if args.target == 'wasm32' else (
        ['-arch', 'x86_64'] if sys.platform == 'darwin' else [])
    flags = [*arch, '-O2', '-std=gnu89', '-fno-builtin', '-fno-strict-aliasing', '-fwrapv',
             '-ffunction-sections', '-fdata-sections', '-Wno-everything',
             '-I' + str(headers), '-Iport/include', '-Iport/core', '-I' + str(hardware / 'include'),
             '-I' + str(hardware / 'hw'), '-include', 'port/core/compat.h']
    addresses = read_addresses(root)
    objects = []
    bindings = {}
    units = CORE + GAME + [str(bios)]
    exports = list(EXPORTS)
    if args.mmio:
        units += ['port/core/bus.c', 'port/core/devices.c', 'src/fn/sub_08213D10.c',
                  'port/core/irq.c', 'src/fn/sub_08243C10.c', 'src/fn/sub_08243D28.c']
        units += [str(p) for p in sorted((hardware / 'hw').glob('*.c')) if p != bios and p.name != 'rtc_sdk.c']
        exports += ['port_devices_init', 'InterruptsAndVideo_Initialize',
                    'port_solar_irq_init', 'port_solar_irq_calls', 'port_irq_raise', 'port_irq_dispatch']
    if args.ppu_root:
        ppu = Path(args.ppu_root).resolve()
        flags += ['-I' + str(ppu / 'include')]
        units += ['port/core/video.c', str(ppu / 'ppu/ppu.c')]
        exports += ['port_video_init', 'port_video_framebuffer', 'port_video_capture_line', 'port_video_capture_framebuffer']
    for name in units:
        source = root / name
        key = name.replace('/', '_').replace('\\', '_').lstrip('_')
        prepared = out / (key + '.c')
        prepared.write_text(prepare_source(source, args.target))
        ir = out / (key + '.ll')
        subprocess.run([args.cc, *flags, '-S', '-emit-llvm', str(prepared), '-o', str(ir)], check=True)
        text, resolved, unresolved = bind_ir(ir.read_text(), addresses)
        bindings[name if not source.is_absolute() or source.is_relative_to(root) else 'port/external/' + source.name] = {
            'resolved': resolved, 'unresolved': unresolved}
        if args.mmio and name.startswith('src/'):
            text, metrics = instrument_mmio(text)
            (out / (key + '.mmio.json')).write_text(json.dumps(metrics))
        ir.write_text(text)
        obj = out / (key + '.o')
        subprocess.run([args.cc, *arch, '-O2', '-c', str(ir), '-o', str(obj)], check=True)
        objects.append(str(obj))
    (out / 'bindings.json').write_text(json.dumps(bindings, indent=2) + '\n')
    if args.target == 'wasm32':
        destination = out / 'boot.wasm'
        command = [find_linker(), '--no-entry', '--gc-sections', '--export-memory',
                   '--initial-memory=167772160', '--max-memory=167772160',
                   '--global-base=1048576', '-z', 'stack-size=1048576',
                   *['--export=' + name for name in exports], *objects, '-o', str(destination)]
    else:
        destination = out / 'boot_test'
        ldflags = ['-Wl,-no_pie,-pagezero_size,0x10000,-dead_strip'] if sys.platform == 'darwin' else ['-no-pie', '-Wl,--gc-sections']
        command = [args.cc, *arch, '-Iport/include', '-Iport/core', 'port/tests/boot_test.c',
                   *objects, *ldflags, '-o', str(destination)]
    subprocess.run(command, check=True)
    print(destination.relative_to(root))
    return 0
