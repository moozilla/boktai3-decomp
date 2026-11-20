"""Project unblended GBA OBJ tiles onto a captured display coordinate."""
import struct

SHAPES = (
    ((8, 8), (16, 16), (32, 32), (64, 64)),
    ((16, 8), (32, 8), (32, 16), (64, 32)),
    ((8, 16), (8, 32), (16, 32), (32, 64)),
)


def word(data, offset):
    return struct.unpack_from("<H", data, offset)[0]


def color555(value):
    channels = [(value >> shift) & 31 for shift in (0, 5, 10)]
    return tuple((channel << 3) | (channel >> 2) for channel in channels)


def tile_xy(oam, attrs, x, y):
    a0, a1, _ = attrs
    shape = a0 >> 14
    affine = bool(a0 & 0x100)
    if shape >= len(SHAPES) or (not affine and a0 & 0x200) or a0 & 0x1C00:
        return None  # hidden, mosaic, semitransparent or object-window
    width, height = SHAPES[shape][a1 >> 14]
    dx = (x - (a1 & 511)) & 511
    dy = (y - (a0 & 255)) & 255
    if affine:
        matrix = (a1 >> 9) & 31
        aa, ab, ac, ad = (struct.unpack_from("<h", oam, 32 * matrix + offset)[0]
                          for offset in (6, 14, 22, 30))
        scale = 2 if a0 & 0x200 else 1
        w, h = scale * width, scale * height
        if dx >= w or dy >= h:
            return None
        dx -= w // 2
        dy -= h // 2
        dx, dy = ((aa * dx + ab * dy) >> 8) + width // 2, ((ac * dx + ad * dy) >> 8) + height // 2
    else:
        if a1 & 0x1000:
            dx = width - 1 - dx
        if a1 & 0x2000:
            dy = height - 1 - dy
    return (dx, dy, width) if 0 <= dx < width and 0 <= dy < height else None


def matching_objects(oam, vram, palette, display, x, y, rgb):
    """Return nontransparent matching fragments, in hardware OBJ priority order."""
    if not display & 0x1000 or display & 0x80:
        return []
    matches = []
    for slot, (a0, a1, a2, _) in enumerate(struct.iter_unpack("<4H", oam)):
        attrs = (a0, a1, a2)
        coords = tile_xy(oam, attrs, x, y)
        if not coords:
            continue
        dx, dy, width = coords
        bpp8 = bool(a0 & 0x2000)
        tile = a2 & 1023
        if bpp8 and not display & 0x40:
            tile &= ~1
        if (display & 7) >= 3 and tile < 512:
            continue
        stride = 2 if bpp8 else 1
        column = (dx // 8) * stride
        if display & 0x40:
            tile += (dy // 8) * (width // 8) * stride + column
        else:
            tile = (tile & ~31) + (dy // 8) * 32 + ((tile + column) & 31)
        tile &= 1023
        pixel_x = dx & 7
        pixel = vram[32 * tile + (dy & 7) * (8 if bpp8 else 4) + (pixel_x if bpp8 else pixel_x // 2)]
        if not bpp8:
            pixel = (pixel >> (4 * (pixel_x & 1))) & 15
        bank = 0 if bpp8 else a2 >> 12
        if pixel and color555(word(palette, 2 * (bank * 16 + pixel))) == rgb:
            matches.append({"slot": slot, "attributes": attrs, "priority": (a2 >> 10) & 3,
                            "tile": tile, "palette_index": bank * 16 + pixel, "texture_xy": [dx, dy]})
    return sorted(matches, key=lambda item: (item["priority"], item["slot"]))
