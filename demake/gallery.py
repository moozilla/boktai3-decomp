"""Contact sheets use reconstructed hardware ink values, including sprite transparency."""
import math
from PIL import Image, ImageDraw, ImageFont

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


def actor_gallery(source, key, frames, path, palette=None):
    total, render = source.actor(key, palette)
    rows = []
    enemy = key in source.big_ids
    for i in frames:
        if not 0 <= i < total:
            raise ValueError(f"frame {i} outside 0..{total - 1}")
        frame = render(i)
        if frame is None:
            frame = Image.new("RGBA", (1, 1))
        pose = actor_pose(frame, enemy)
        previews = []
        for shade in (0, 85, 170, 255):
            bg = Image.new("RGB", (32, 36), (shade,) * 3)
            ink = pose_image(pose, enemy)
            bg.paste(ink, (8, 2), ink)
            previews.append((f"floor {shade}", bg.resize((96, 108), Image.Resampling.NEAREST)))
        orig = Image.new("RGB", (96, 108), (95, 101, 101))
        frame.thumbnail(orig.size, Image.Resampling.NEAREST)
        orig.paste(frame, ((96 - frame.width) // 2, 106 - frame.height), frame)
        rows.append((f"{key:04X}  /  frame {i} of {total}", [("source", orig), *previews]))
    contact(rows, path)


def room_gallery(rooms, path, budget=208):
    rows = []
    for r in rooms:
        tiles, _, display, n = fit_tiles(r.background, budget)
        mp = r.stage["stage"]
        views = [("Source palette " + r.palette, r.color),
                 ("Four inks", screen_image(r.background)),
                 (f"{len(tiles)} hardware patterns from {n}", screen_image(display)),
                 ("HP floor edges and ramps", terrain_overlay(r))]
        rows.append((f"SCENE {r.stage['scene']}    MP {mp['map']}    PTN {mp['tileset']}    HP {mp['hp']}", views))
    contact(rows, path)
