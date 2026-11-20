"""Contact sheets use reconstructed hardware ink values, including sprite transparency."""
from PIL import Image, ImageDraw

from art import actor_pose, pose_image, fit_tiles, screen_image
from rooms import terrain_overlay


def contact(rows, path):
    # Each record is a caption and a row of labeled, native-resolution views.
    width = max(sum(im.width + 12 for _, im in views) for _, views in rows) + 16
    height = sum(58 + max(im.height for _, im in views) for _, views in rows) + 8
    out = Image.new("RGB", (width, height), (244, 243, 239))
    draw = ImageDraw.Draw(out)
    y = 8
    for title, views in rows:
        draw.text((12, y), title, fill=(20, 23, 24))
        x = 12
        for label, im in views:
            out.paste(im, (x, y + 19))
            draw.text((x, y + 22 + im.height), label, fill=(62, 65, 64))
            x += im.width + 12
        y += 58 + max(im.height for _, im in views)
    out.save(path)


def actor_gallery(source, key, frames, path, palette=None, size=(24, 32), scale=.85, floor=None, enemy=None):
    total, render = source.actor(key, palette)
    rows = []
    if enemy is None:
        enemy = key in source.big_ids
    viewport = (size[0] + 12, size[1] + 8)
    zoom = (viewport[0] * 3, viewport[1] * 3)
    for i in frames:
        if not 0 <= i < total:
            raise ValueError(f"frame {i} outside 0..{total - 1}")
        frame = render(i)
        if frame is None or not frame.getbbox():
            frame = Image.new("RGBA", (1, 1))
        else:
            frame = frame.crop(frame.getbbox())
        pose = pose_image(actor_pose(frame, size=size, scale=scale), enemy)
        previews = []
        backgrounds = [(f"floor {shade}", Image.new("RGB", viewport, (shade,) * 3)) for shade in (0, 85, 170, 255)]
        if floor is not None:
            x, y = floor.width // 2, floor.height // 2
            backgrounds.append(("cartridge floor", floor.crop((x - viewport[0]//2, y - viewport[1]//2,
                                x - viewport[0]//2 + viewport[0], y - viewport[1]//2 + viewport[1])).convert("RGB")))
        for label, bg in backgrounds:
            bg.paste(pose, (6, 4), pose)
            previews.append((label, bg.resize(zoom, Image.Resampling.NEAREST)))
        original = Image.new("RGB", zoom, (95, 101, 101))
        factor = min(zoom[0] / frame.width, (zoom[1]-8) / frame.height, 3)
        frame = frame.resize((max(1, round(frame.width*factor)), max(1, round(frame.height*factor))), Image.Resampling.NEAREST)
        original.paste(frame, ((zoom[0] - frame.width) // 2, zoom[1] - 4 - frame.height), frame)
        rows.append((f"{key:04X}   /   frame {i} of {total}   /   {size[0]} x {size[1]} DMG   /   OBP{int(enemy)}",
                     [("GBA source", original), *previews]))
    contact(rows, path)


def room_gallery(rooms, path, budgets):
    rows = []
    for r, budget in zip(rooms, budgets, strict=True):
        tiles, _, display, n = fit_tiles(r.background, budget)
        mp = r.stage["stage"]
        views = [("Source palette " + r.palette, r.color),
                 ("Four inks before tile fitting", screen_image(r.background)),
                 (f"{len(tiles)} hardware patterns from {n}", screen_image(display)),
                 ("HP floor edges and ramps", terrain_overlay(r))]
        rows.append((f"SCENE {r.stage['scene']}    MP {mp['map']}    PTN {mp['tileset']}    HP {mp['hp']}", views))
    contact(rows, path)
