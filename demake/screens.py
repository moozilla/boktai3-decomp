"""Compose full-screen DMG title and light meter pages from ROM art and font."""
import numpy as np
from PIL import Image, ImageDraw

from art import SHADES, encode_tile, fit_tiles, lightness, quantize_bg, room_ramp, unpack_4bpp
from pack import icon


class Lettering:
    """Variable advances from the ROM's half-width face; keep a shared baseline."""
    def __init__(self, source):
        raw = source.entry("fnt")[12:12 + 128 * 64]
        self.glyph = unpack_4bpp(raw).reshape(128, 16, 8)

    def text(self, page, xy, value, fill=255, height=9, center=False):
        glyphs = []
        for ch in value:
            tile = self.glyph[ord(ch) - 32]
            alpha = Image.fromarray(np.uint8((4 - tile) * 85))
            if alpha.getbbox() is None:
                glyphs.append((None, max(2, height // 3)))
                continue
            x0, _, x1, _ = alpha.getbbox()
            alpha = alpha.crop((x0, 1, x1, 15)).resize((max(1, round((x1 - x0) * height / 13)), height),
                                                        Image.Resampling.BOX)
            alpha = alpha.point(lambda v: 255 if v > 65 else 0)
            glyphs.append((alpha, alpha.width + 1))
        width = sum(n for _, n in glyphs)
        x, y = xy
        if center:
            x -= width // 2
        if x < 0 or x + width > 161:
            raise ValueError(f"text exceeds DMG page: {value} ({width}px)")
        for glyph, advance in glyphs:
            if glyph:
                page.paste(fill, (x, y, x + glyph.width, y + height), glyph)
            x += advance


def stamp(page, image, box, invert=False):
    image = image.crop(image.getbbox())
    image.thumbnail((box[2], box[3]), Image.Resampling.LANCZOS)
    rgba = np.asarray(image)
    v = lightness(rgba[:, :, :3])
    mask = rgba[:, :, 3] >= 100
    light = np.clip((v - np.percentile(v[mask], 4)) / max(.2, np.ptp(v[mask])), 0, 1)
    if invert:
        light = 1 - light
    ink = Image.fromarray(SHADES[quantize_bg(np.uint8(light * 255))])
    x = box[0] + (box[2] - image.width) // 2
    y = box[1] + (box[3] - image.height) // 2
    page.paste(ink, (x, y), Image.fromarray(np.uint8(mask) * 255))


def title(source, font):
    _, logo = source.actor(0x731D)
    mp = source.map("C5A7", None)
    _, pal = source.palette({"stage": {"map": "C5A7"}})
    colors = pal[mp[:160, :240]]
    sky = Image.fromarray(np.uint8(lightness(colors) * 255)).resize((160, 96), Image.Resampling.LANCZOS)
    page = Image.new("L", (160, 144), 0)
    page.paste(Image.fromarray(SHADES[quantize_bg(sky)]), (0, 0))
    # The original flame and the separate title/subtitle packages.
    stamp(page, logo(1), (4, 29, 152, 55))
    stamp(page, logo(2), (20, 78, 120, 16))
    font.text(page, (80, 102), "BOKTAI 3", height=12, center=True)
    stamp(page, logo(11), (28, 121, 104, 12), invert=True)
    return page


def meter(source, font):
    page = Image.new("L", (160, 144), 0)
    d = ImageDraw.Draw(page)
    _, otenko = source.actor(0x97D3)
    stamp(page, otenko(0), (126, 5, 28, 29))
    font.text(page, (7, 5), "SUN METER", height=14)
    font.text(page, (7, 25), "Shade the LCD with a thumb.")
    font.text(page, (7, 37), "Match the edge you see.")
    # A pale reflective field with three printed shadow-edge references.
    for i, label in enumerate(("NONE", "SOFT", "SHARP")):
        x = 8 + i * 48
        d.rectangle((x, 54, x + 42, 85), fill=255)
        if i:
            for yy in range(55, 85):
                for xx in range(x+1, x+42):
                    distance = ((xx-x-20)/15)**2 + ((yy-72)/8)**2
                    if distance < .78:
                        tone = 85 if i == 1 else 0
                    elif distance < 1.55 and i == 1:
                        tone = 170 if (xx+yy) % 2 else 255
                    else:
                        continue
                    d.point((xx, yy), fill=tone)
        font.text(page, (x + 21, 88), label, center=True)
    d.line((7, 110, 153, 110), fill=85)
    font.text(page, (7, 115), "A  TAKE READING      B  BACK")
    font.text(page, (7, 130), "START  CAMERA LINK", fill=170)
    return page


def photo_link(font):
    page = Image.new("L", (160, 144), 0)
    d = ImageDraw.Draw(page)
    font.text(page, (7, 5), "SUN PRINT", height=14)
    d.line((7, 24, 153, 24), fill=170)
    font.text(page, (7, 31), "Link a second Game Boy.")
    font.text(page, (7, 43), "On its Camera, photograph")
    font.text(page, (7, 55), "a pale card in the light.")
    font.text(page, (7, 72), "Choose PRINT on the Camera.")
    font.text(page, (7, 84), "Bright print, strong sunlight.")
    font.text(page, (7, 102), "EXPOSURE")
    for n in range(4):
        d.rectangle((66 + 18*n, 102, 82 + 18*n, 109), fill=85*n)
    font.text(page, (7, 129), "A  USE READING     B  BACK")
    return page


CURSOR = icon("33300333 33000033 30000003 33300333 33300333 33333333 33333333 33333333")
METER_FILLED = icon("33333333 30000003 30000003 30000003 30000003 30000003 33333333 33333333")


def screen_bank(source):
    font = Lettering(source)
    pages = [title(source, font), meter(source, font), photo_link(font)]
    data = bytearray()
    for page in pages:
        pix = (3 - np.rint(np.asarray(page) / 85)).astype(np.uint8)
        patterns, ids, _, _ = fit_tiles(pix, 253)
        charset = b"".join(encode_tile(t) for t in patterns).ljust(253 * 16, b"\0")
        charset += b"".join(encode_tile(t) for t in (CURSOR, np.full((8, 8), 3), METER_FILLED))
        bgmap = np.zeros((32, 32), np.uint8)
        bgmap[:18, :20] = ids.reshape(18, 20) ^ 128
        data.extend(charset + bgmap.tobytes())
    return bytes(data)
