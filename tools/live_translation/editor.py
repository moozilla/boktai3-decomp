"""An SDL text editor for the line the cartridge is currently displaying."""
import ctypes as C
from functools import lru_cache
from PIL import Image, ImageDraw, ImageFont

from translator import CONTROLS, fit


@lru_cache(maxsize=24)
def face(size, japanese=False, bold=False):
    names = (["Hiragino Sans GB.ttc", "PingFang.ttc", "NotoSansCJK-Regular.ttc"]
             if japanese else ["HelveticaNeue.ttc", "DejaVuSans.ttf"])
    if bold and not japanese:
        names = ["Arial Bold.ttf", "DejaVuSans-Bold.ttf"] + names
    for name in names:
        try:
            return ImageFont.truetype(name, size)
        except OSError:
            pass
    return ImageFont.load_default(size=size)


def wrap(text, font, width):
    """Visual row ranges preserve caret positions and explicit newlines."""
    rows = []
    start = 0
    for pos, char in enumerate(text):
        if char == "\n":
            rows.append((start, pos))
            start = pos+1
        elif font.getlength(text[start:pos+1]) > width and pos > start:
            space = text.rfind(" ", start, pos)
            end = space+1 if space >= start else pos
            rows.append((start, end))
            start = end
    rows.append((start, len(text)))
    return rows


def render(frame, line, buffer, cursor, selected, origin, error):
    ctx = line["context"]
    im = Image.new("RGB", (900,600), "#f7f2e7")
    d = ImageDraw.Draw(im)
    dark, dim, accent = "#252321", "#675f55", "#913e35"
    d.text((20,12), "Edit translation", font=face(24,bold=True), fill=dark)
    geometry = (f'{ctx["width_px"]} x {ctx["height_px"]} pixels' if ctx["kind"] == "graphic"
                else f'{ctx["columns"]} columns, {ctx["lines"]} lines')
    key = f'{line["key"]}   /   {geometry}'
    d.text((880,19), key, anchor="rt", font=face(16), fill=dim)
    im.paste(frame.resize((480,320), Image.Resampling.NEAREST), (20,58))
    x = 520
    d.text((x,57), ctx.get("scene_name") or f'Scene {ctx["scene"]}', font=face(22,bold=True), fill=dark)
    label = ctx.get("speaker_label")
    if label:
        d.text((x,87), label, font=face(16), fill=accent)
    d.text((x,124), "Japanese", font=face(15,bold=True), fill=dim)
    source = CONTROLS.sub("",line["source"]).strip()
    jp = face(18,japanese=True)
    for i,(a,b) in enumerate(wrap(source, jp, 355)[:6]):
        d.text((x,151+i*25), source[a:b], font=jp, fill=dark)
    if "preview" in line:
        with Image.open(line["preview"]) as source_art:
            crop = source_art.crop(ctx["region"]).convert("RGBA")
        zoom = min(3, 350/crop.width, 110/crop.height)
        crop = crop.resize((max(1,int(crop.width*zoom)),max(1,int(crop.height*zoom))), Image.Resampling.NEAREST)
        im.paste(crop, (x,202), crop)
    script = ctx.get("script") or {}
    provenance = origin or "Awaiting translation"
    d.text((x,326), provenance, font=face(15), fill=dim)
    if script:
        d.text((x,349), f'Script {script["procedure"]}   ·   {ctx["caller_pc"]}', font=face(14), fill=dim)
    d.text((20,392), "English", font=face(16,bold=True), fill=accent)
    d.rectangle((20,422,880,528), fill="#fffdf7", outline="#ad9680", width=2)
    wordface = face(19)
    lines = wrap(buffer, wordface, 836)
    caret_row = max(i for i, (a,b) in enumerate(lines) if a <= cursor)
    top = max(0, caret_row-3)
    for i,(a,b) in enumerate(lines[top:top+4]):
        y=429+i*23
        content=buffer[a:b]
        if selected and content:
            d.rectangle((28,y,28+wordface.getlength(content),y+22), fill="#e6cbc0")
        d.text((28,y), content, font=wordface, fill=dark)
        if i+top == caret_row and not selected:
            cx = 28 + wordface.getlength(buffer[a:cursor])
            d.line((cx,y+2,cx,y+22), fill=accent, width=2)
    if error:
        d.text((20,538), error, font=face(15), fill=accent)
    d.text((20,570), "Enter  Save and replay     Shift + Enter  New line     Esc  Cancel",
           font=face(16), fill=dim)
    return im.convert("RGBA").tobytes()


def edit(ui, game):
    resolver = game.resolver
    line = resolver.current_choice(game)
    if not line:
        return True
    row = resolver.memory.db.execute("SELECT english, origin FROM entries WHERE key=?", (line["key"],)).fetchone()
    if row is None:
        return True
    value = row["english"] or "".join(CONTROLS.findall(line["source"]))
    pos, selected = len(value), True
    image = game.image()
    if not ui.lt_begin_edit():
        raise RuntimeError("Could not open the SDL text editor")
    error, redraw = "", True
    try:
        while True:
            if redraw:
                rgba = render(image, line, value, pos, selected, row["origin"], error)
                ui.lt_draw_edit(rgba)
            data = C.create_string_buffer(8192)
            event = ui.lt_edit_event(data, len(data))
            redraw = bool(event)
            if event in (0, 13):
                continue
            if event == 12:
                ui.lt_copy_edit(value.encode())
                continue
            if event in (10, 11):
                return event == 10
            if event == 9:
                try:
                    fit(line["source"], value, line["context"])
                    resolver.memory.correct(line["key"], value)
                except ValueError as exc:
                    error = str(exc)
                    continue
                resolver.source_preview = None
                resolver.original_art = None
                game.rewind_line(line.get("slot",0))
                return True
            error = ""
            if event == 8:
                selected = True
                continue
            if event in (1,2,3):
                if selected:
                    value, pos = "", 0
                if event == 1:
                    inserted = data.value.decode(errors="replace").replace("\r\n", "\n")
                    value = value[:pos]+inserted+value[pos:]
                    pos += len(inserted)
                elif event == 2 and pos:
                    value = value[:pos-1]+value[pos:]
                    pos -= 1
                elif event == 3:
                    value = value[:pos]+value[pos+1:]
            elif event == 4:
                pos = 0 if selected else max(0, pos-1)
            elif event == 5:
                pos = len(value) if selected else min(len(value), pos+1)
            elif event == 6:
                pos = 0
            elif event == 7:
                pos = len(value)
            selected = False
    finally:
        ui.lt_end_edit()
