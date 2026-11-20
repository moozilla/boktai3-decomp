"""Compile room plans and a real GCL message into banked, paged field notes."""
from functools import lru_cache
import re
import struct

import numpy as np
from PIL import Image, ImageDraw

from art import ROOT, encode_tile, fit_tiles, unpack_4bpp
from pack import ROOM_NAMES
from rooms import room_events, walkable
from screens import Lettering, stamp, title
import text as game_text


def page_bytes(page):
    inks = np.uint8(3 - np.rint(np.asarray(page) / 85))
    tiles, ids, _, unique = fit_tiles(inks, 256)
    if unique > 256:
        raise ValueError("field notes exceed 256 hardware background tiles")
    patterns = b"".join(encode_tile(t) for t in tiles)
    return struct.pack("<H", len(patterns)) + patterns + (ids ^ 128).tobytes()


class Journal:
    def __init__(self, source, first_bank):
        self.source = source
        self.first_bank = first_bank
        self.font = Lettering(source)
        self.glyphs = source.entry("fnt")
        self.charmap = game_text.charmap()
        self.reverse = {c: code for code, c in self.charmap.items()}
        self.translation = {}
        translation = ROOT / "build/text/english.tsv"
        if translation.is_file():
            for line in translation.read_text().splitlines():
                fields = line.split("\t", 2)
                if len(fields) == 3 and fields[0].isdigit() and fields[1] == "T":
                    self.translation[int(fields[0])] = fields[2].replace("\\n", "\n")
        self.banks = []
        self.directory = bytearray()
        _, render = source.actor(0x97D3)
        self.otenko = render(0)

    def append(self, page):
        data = page_bytes(page)
        if not self.banks or len(self.banks[-1]) + len(data) > 0x4000:
            self.banks.append(bytearray())
        bank = self.banks[-1]
        self.directory.extend(struct.pack("<BH", self.first_bank + len(self.banks)-1, 0x4000 + len(bank)))
        bank.extend(data)

    def raw_text(self, index):
        offset, = struct.unpack_from("<I", self.source.rom, game_text.TABLE - 0x08000000 + 4 * index)
        if not offset & 0x80000000:
            return ""
        start = game_text.BASE - 0x08000000 + (offset & 0x7fffffff)
        end = self.source.rom.index(b"\0", start)
        return game_text.decode(self.source.rom[start:end], self.charmap)

    @lru_cache(maxsize=2048)
    def glyph(self, char):
        if 32 <= ord(char) < 127:
            data = self.font.glyph[ord(char) - 32]
        else:
            if char not in self.reverse:
                char = "?"
                data = self.font.glyph[ord(char)-32]
            else:
                start = 0x200c + 128*self.reverse[char]
                blocks = unpack_4bpp(self.glyphs[start:start+128])
                data = blocks.reshape(2, 2, 8, 8).transpose(0, 2, 1, 3).reshape(16, 16)
        alpha = Image.fromarray(np.uint8((4 - data) * 85))
        box = alpha.getbbox()
        if box is None:
            return None, 4
        glyph = alpha.crop((box[0], 0, box[2], 16))
        # Original height and two-bit antialiasing preserve small kanji strokes.
        return glyph, glyph.width + 1

    def wrap(self, body):
        lines, line, width = [], "", 0
        for word in re.findall(r"[!-~]+|[^\x00-\x7f]|\s+", body.replace("\n", " ")):
            if word.isspace():
                word = " " if line and line[-1].isascii() else ""
            advance = sum(self.glyph(c)[1] for c in word)
            if line and width + advance > 144:
                lines.append(line.rstrip())
                line, width = "", 0
            for ch in word.lstrip() if not line else word:
                w = self.glyph(ch)[1]
                if width + w > 144:
                    lines.append(line.rstrip())
                    line, width = "", 0
                line += ch
                width += w
        if line.strip():
            lines.append(line.strip())
        return [lines[i:i+4] for i in range(0, len(lines), 4)]

    def dialogue(self, start, english):
        if start is None:
            return []
        result = []
        for n in range(start, min(start+64, game_text.COUNT)):
            original = self.raw_text(n)
            if not original or n > start and original.startswith("<LABEL="):
                break
            body = self.translation.get(n, original) if english else original
            body = re.sub(r"<LABEL=[^>]+>.*?</LABEL>\s*[:：]?", "", body, flags=re.S)
            body = body.replace("<NAME>", "Django" if english else "ジャンゴ")
            body = re.sub("<[^>]+>", "", body).strip()
            body = body.translate(str.maketrans({"’": "'", "“": '"', "”": '"', "—": "-", "…": "…"}))
            if body:
                result.extend((n, lines) for lines in self.wrap(body))
            if "<END>" in original:
                break
        return result

    def text_page(self, entry, page_number, count, language, bilingual, speaker):
        _, lines = entry
        canvas = Image.new("L", (160, 144))
        draw = ImageDraw.Draw(canvas)
        title = "OTENKO" if speaker == "OTENKO" else "FIELD NOTES"
        self.font.text(canvas, (8, 5), title, height=12)
        self.font.text(canvas, (8, 22), f"{language}   {page_number:02d} / {count:02d}", fill=170)
        if speaker == "OTENKO":
            stamp(canvas, self.otenko, (122, 2, 31, 28))
        draw.line((8, 35, 151, 35), fill=85)
        for i, line in enumerate(lines):
            x, y = 8, 40 + 18*i
            for c in line:
                glyph, delta = self.glyph(c)
                if glyph:
                    canvas.paste(255, (x, y, x+glyph.width, y+glyph.height), glyph)
                x += delta
        draw.line((8, 118, 151, 118), fill=85)
        self.font.text(canvas, (8, 126), "A NEXT   B MAP" + ("   SELECT JP/EN" if bilingual else ""))
        return canvas

    def room_map(self, room, rooms, events, has_text):
        scene = room.stage["scene"]
        page = Image.new("L", (160, 144))
        d = ImageDraw.Draw(page)
        self.font.text(page, (7, 4), ROOM_NAMES.get(scene, "FLOOR MAP"), height=12)
        self.font.text(page, (7, 20), f"ROOM  {rooms[scene][1]:02d} / {len(rooms):02d}", fill=170)
        d.line((7, 32, 153, 32), fill=85)
        t = room.terrain
        w, h = t["width"], t["height"]
        unit = max(1, min(9, 96//w, 80//h))
        left, top = 5 + (98 - w*unit)//2, 35 + (82 - h*unit)//2
        for i, cell in enumerate(t["cells"]):
            if not walkable(cell):
                continue
            x, y = left + i % w * unit, top + i // w * unit
            shade = 170 if cell[1] else 85
            d.rectangle((x, y, x+unit-1, y+unit-1), fill=shade)
            # Outline each exposed edge, leaving internal floors joined.
            for dx, dz in ((0,-1),(1,0),(0,1),(-1,0)):
                a,b = i%w+dx, i//w+dz
                if 0 <= a < w and 0 <= b < h and walkable(t["cells"][a+b*w]):
                    continue
                if dx:
                    xx = x+(unit-1 if dx>0 else 0)
                    d.line((xx,y,xx,y+unit-1),fill=170)
                else:
                    yy = y+(unit-1 if dz>0 else 0)
                    d.line((x,yy,x+unit-1,yy),fill=170)
        def symbol(x,y,kind):
            if kind == 1:
                d.rectangle((x-2,y-2,x+2,y+2),fill=0,outline=255)
            elif kind == 3:
                d.line((x-2,y,x+2,y),fill=255)
                d.line((x,y-2,x,y+2),fill=255)
            else:
                d.polygon(((x,y-2),(x+2,y),(x,y+2),(x-2,y)),fill=255)
        for kind,x0,z0,x1,z1,_,_,_ in events:
            symbol(left + (x0+x1)*unit//16, top + (z0+z1)*unit//16, kind)
        for y, name, kind in ((55,"DOOR",1),(69,"FRUIT",2),(83,"SUN",3)):
            symbol(112,y,kind)
            self.font.text(page,(119,y-4),name)
        self.font.text(page, (108, 101), "YOU", fill=170)
        d.polygon(((105,101),(105,107),(100,104)),fill=255)
        d.line((7, 119, 153, 119), fill=85)
        self.font.text(page, (7, 128), ("A READ   " if has_text else "") + "SELECT SAVE   B BACK")
        # Hardware pointer tile is centered on Django's actual X/Z sample.
        xs = bytes(min(255, left + x*unit//8 + 5) for x in range(256))
        ys = bytes(min(255, top + z*unit//8 + 5) for z in range(256))
        return page, xs+ys

    def system_pages(self):
        saved = Image.new("L", (160, 144))
        stamp(saved, self.otenko, (62, 11, 36, 36))
        self.font.text(saved, (80, 54), "CHECKPOINT SAVED", center=True, height=12)
        self.font.text(saved, (80, 79), "Press Start at the title", center=True)
        self.font.text(saved, (80, 92), "to return to this spot.", center=True)
        self.font.text(saved, (80, 124), "A  MAP     START  RETURN", center=True, fill=170)
        saved_offset = len(self.directory)
        self.append(saved)
        resume = title(self.source, self.font)
        ImageDraw.Draw(resume).rectangle((0, 119, 159, 143), fill=0)
        self.font.text(resume, (80, 127), "START  CONTINUE     A  NEW", center=True)
        resume_offset = len(self.directory)
        self.append(resume)
        return saved_offset, resume_offset

    def room(self, room, rooms):
        index = len(self.directory)
        text_id = room.stage["message"]
        jp = self.dialogue(text_id, False)
        en = self.dialogue(text_id, True) if text_id in self.translation else []
        source_text = self.raw_text(text_id) if text_id else ""
        match = re.match(r"<LABEL=([^>]+)>", source_text)
        speaker = match[1] if match else ""
        page, coords = self.room_map(room, rooms, room_events(room, rooms), bool(jp))
        self.append(page)
        for language, pages in (("JP", jp), ("EN", en)):
            for i, entry in enumerate(pages):
                self.append(self.text_page(entry, i+1, len(pages), language, bool(en), speaker))
        return dict(offset=index, jp=len(jp), en=len(en), text=text_id), coords
