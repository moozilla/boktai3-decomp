#!/usr/bin/env python3
"""Create uncommitted PNG/APNG comparisons from locally captured PPU scenes.

Run after the README capture recipes. No pixels or game data belong in port/demo.
"""
import argparse
from pathlib import Path
from PIL import Image, ImageChops, ImageDraw


def comparison(directory, name, label):
    actual = Image.open(directory / (name + '.port.ppm')).convert('RGB')
    expected = Image.open(directory / (name + '.ppm')).convert('RGB')
    if actual.size != (240, 160) or expected.size != actual.size:
        raise ValueError('Expected 240 by 160 frames')
    diff = ImageChops.difference(actual, expected)
    count = sum(any(pixel) for pixel in zip(*[iter(diff.tobytes())] * 3))
    canvas = Image.new('RGB', (1008, 398), '#0d1520')
    draw = ImageDraw.Draw(canvas)
    draw.text((16, 12), label, fill='#efbd5d')
    draw.text((16, 38), 'Portable C / native', fill='#eaf1f8')
    draw.text((512, 38), 'mGBA / same frame', fill='#eaf1f8')
    canvas.paste(actual.resize((480, 320), Image.Resampling.NEAREST), (16, 62))
    canvas.paste(expected.resize((480, 320), Image.Resampling.NEAREST), (512, 62))
    draw.text((680, 12), f'{count} / 38400 pixel mismatches', fill='#efbd5d')
    return canvas, count


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out', type=Path, default=Path('build/demo'))
    ap.add_argument('--motion', type=Path, default=Path('build/demo/motion'))
    args = ap.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    selected = [
        ('title', Path('build/ppu/startup-delta'), 'startup_11', 'Title sequence'),
        ('menu', Path('build/ppu/menus'), '01_valuables', 'Game menu'),
        ('dialogue', Path('build/ppu/story/0013'), 'field', 'Dungeon dialogue'),
        ('snow', Path('build/ppu/story/0032'), 'field', 'Snow field / sun gauge HUD'),
        ('castle', Path('build/ppu/story/0069'), 'field', 'Castle field'),
        ('boss', Path('build/ppu/menus'), '12_field', 'Live field / boss encounter'),
    ]
    rows, images = [], []
    for slug, directory, name, label in selected:
        if not (directory / (name + '.ppm')).exists():
            print(f'Skipping unavailable scene {directory}/{name}')
            continue
        frame, count = comparison(directory, name, label)
        frame.save(args.out / f'{slug}.png')
        images.append(frame)
        rows.append(f'- `{slug}.png`: {label}; native C and mGBA, {count} mismatched pixels.')
    if images:
        gallery = Image.new('RGB', (1008, 398 * len(images)), '#0d1520')
        for i, frame in enumerate(images):
            gallery.paste(frame, (0, 398 * i))
        gallery.save(args.out / 'gallery.png')
        rows.append('- `gallery.png`: all selected comparisons in one image.')
    frames = []
    for path in sorted(args.motion.glob('motion_*.port.ppm')):
        name = path.name.removesuffix('.port.ppm')
        frame, count = comparison(args.motion, name, 'Snow field animation / capture every 4 GBA frames')
        if count:
            raise ValueError(f'{name}: {count} mismatches')
        frames.append(frame)
    if frames:
        frames[0].save(args.out / 'motion.apng', save_all=True, append_images=frames[1:], duration=67, loop=0)
        rows.append(f'- `motion.apng`: {len(frames)} native C frames beside their mGBA references, approximately 15 fps; all exact.')
        rows.append('- `motion/`: numbered capture files and reference PPMs; select these together in the browser demo to run the same sequence in WebAssembly.')
    (args.out / 'README.md').write_text('# PPU demo artifacts\n\n' + '\n'.join(rows) + '\n\n'
        'Open `port/demo/index.html` directly in a browser. Select `.ppu.gz` files and matching `.ppm` references from `motion/` or `build/ppu/`. The browser executes the C renderer compiled to WebAssembly; no server is needed.\n\n'
        'Recreate the moving captures with:\n\n```sh\nbuild/venv/bin/python port/tests/ppu_regression.py --capture port/tests/ppu_motion.txt --save build/ppu/story/0032/input.sav --out build/demo/motion --wasm\nbuild/venv/bin/python port/demo/artifacts.py\n```\n\n'
        'The battery save comes from the user-provided checkpoint bank via `port/tests/ppu_scenes.py`. These images, captures and animation are ROM-derived and must remain uncommitted. The committed browser demo and its module contain only original renderer code.\n')
    print(f'Wrote {len(images)} comparisons and {len(frames)} animation frames to {args.out}')


if __name__ == '__main__':
    main()
