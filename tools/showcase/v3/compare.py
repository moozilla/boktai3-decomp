#!/usr/bin/env python3
"""Line sprites up next to the game's own for a style check: 1x and 4x.

  compare.py OUT.png REF.png... -- NEW.png...

REF and NEW are 1x RGBA images (for the game's own, render them with the
Studio or sprites.py); they are drawn on a floor-colored strip at 1x and the
same strip scaled 4x below it.
"""
import sys

from PIL import Image


def main():
    out = sys.argv[1]
    k = sys.argv.index("--")
    refs, new = sys.argv[2:k], sys.argv[k + 1:]
    ims = [Image.open(p).convert("RGBA") for p in refs + new]
    w = sum(im.width + 6 for im in ims) + 6
    h = max(im.height for im in ims) + 8
    strip = Image.new("RGBA", (w, h), (92, 84, 72, 255))
    x = 6
    for i, im in enumerate(ims):
        if i == len(refs):
            x += 8
        strip.alpha_composite(im, (x, h - 4 - im.height))
        x += im.width + 6
    big = strip.resize((w * 4, h * 4), Image.NEAREST)
    sheet = Image.new("RGBA", (big.width, h + 8 + big.height), (40, 40, 48, 255))
    sheet.alpha_composite(strip, (0, 0))
    sheet.alpha_composite(big, (0, h + 8))
    sheet.save(out)


if __name__ == "__main__":
    main()
