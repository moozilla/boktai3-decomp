#!/usr/bin/env python3
"""Create local PNG/GIF comparisons from wasm-rendered PPMs and mGBA references."""
import argparse
import json
from pathlib import Path
from PIL import Image, ImageChops, ImageDraw


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('captures', type=Path)
    parser.add_argument('rendered', type=Path)
    parser.add_argument('--output', type=Path, default=Path('build/demo'))
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    frames, rows = [], []
    for source in sorted(args.rendered.glob('*.wasm.ppm')):
        name = source.name.removesuffix('.wasm.ppm')
        actual = Image.open(source).convert('RGB')
        reference = Image.open(args.captures / (name + '.ppm')).convert('RGB')
        if actual.size != (240, 160) or reference.size != actual.size:
            raise ValueError('Expected 240x160 frames')
        difference = ImageChops.difference(actual, reference)
        mismatch = sum(pixel != (0, 0, 0) for pixel in difference.getdata())
        sheet = Image.new('RGB', (1000, 414), '#101a22')
        draw = ImageDraw.Draw(sheet)
        draw.text((16, 14), 'BOKTAI 3 / C SOURCE PORT', fill='#ffc56e')
        draw.text((16, 40), 'WASM C RENDERER', fill='white')
        draw.text((516, 40), 'mGBA / SAME CAPTURED FRAME', fill='white')
        sheet.paste(actual.resize((480, 320), Image.Resampling.NEAREST), (16, 65))
        sheet.paste(reference.resize((480, 320), Image.Resampling.NEAREST), (516, 65))
        draw.text((16, 394), f'{name} | {mismatch} / 38,400 pixels differ | recorded state replay', fill='#88e0bd')
        sheet.save(args.output / (name + '-comparison.png'))
        actual.save(args.output / (name + '-wasm.png'))
        reference.save(args.output / (name + '-mgba.png'))
        frames.append(sheet)
        rows.append({'frame':name, 'mismatched_pixels':mismatch})
    if not frames: raise ValueError('No wasm-rendered PPM frames')
    if len(frames) > 1:
        # One shared palette prevents identical colors changing between frames.
        palette = frames[0].quantize(colors=256)
        gif = [frame.quantize(palette=palette, dither=Image.Dither.NONE) for frame in frames]
        gif[0].save(args.output / 'comparison.gif', save_all=True, append_images=gif[1:],
                    duration=67, loop=0, disposal=2)
        frames[0].save(args.output / 'comparison.apng', save_all=True,
                       append_images=frames[1:], duration=1000 / 15, loop=0)
    (args.output / 'artifact-comparison.json').write_text(json.dumps(rows, indent=2) + '\n')
    print(f'{len(frames)} wasm/reference comparisons; {sum(r["mismatched_pixels"] for r in rows)} differing pixels')


if __name__ == '__main__': main()
