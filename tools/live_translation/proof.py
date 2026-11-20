"""Replay semantic checkpoints twice, retaining raw GBA frames and encounter evidence."""
from contextlib import closing
import json

from PIL import Image, ImageChops, ImageDraw, ImageFont
from player import BOOT
from translator import Memory

from frames import ArtTranslator
from localize import Localizer
from runtime import Game, OUT, ROOT, prison_save, saves

ROWS = {
    "00-sunlight": "Cartridge sunlight advisory / baked background",
    "01-title": "Title logo and subtitle",
    "02-file": "File selection and button legends",
    "03-prison": "Sealed Prison / first voice",
    "04-awake": "Otenko speaks the saved name",
    "05-blood": "Dialogue fitted to the message window",
    "06-config": "System settings",
    "07-items": "Item name and scrolling description",
}
def session(memory, save, english, stream):
    loc = Localizer(memory, stream, enabled=english)
    loc.graphics = ArtTranslator()
    loc.bookmarks = english
    return closing(Game(loc, save))


def capture(game, directory, label):
    game.image().save(directory / (label + ".png"))


def correction(game, directory):
    """Replay one captured fetch three ways at exactly the same emulated time."""
    where = directory / "correction"
    where.mkdir(exist_ok=True)
    loc = game.resolver
    original_memory = loc.memory
    private = Memory(":memory:")
    original_memory.db.backup(private.db)
    loc.memory = private
    key = loc.line["key"]
    try:
        def retake(name):
            game.rewind_line()
            game.step(260)
            image = game.image()
            image.save(where / (name+".png"))
            return image
        english = retake("01-english")
        loc.source_preview = loc.line["id"]
        japanese = retake("02-japanese")
        loc.source_preview = None
        replacement = "That vampire blood kept you alive."
        row = private.db.execute("SELECT english FROM entries WHERE key=?", (key,)).fetchone()
        if row and row["english"] == replacement:
            replacement = "Your vampire blood saved your life."
        private.correct(key, replacement, author="scripted proof correction")
        changed = retake("03-corrected")
        for other in (japanese, changed):
            bounds = ImageChops.difference(english, other).getbbox()
            if not bounds or bounds[1] < 100:
                raise RuntimeError(f"Unexpected change outside the dialogue window: {bounds}")
        if loc.line.get("english") != replacement:
            raise RuntimeError("The corrected line did not reach the renderer")
        from editor import render
        page = render(changed, loc.line, replacement, len(replacement), False, "scripted proof correction", "")
        Image.frombytes("RGBA", (900,600), page).save(where / "04-editor.png")
    finally:
        loc.memory = original_memory
        private.db.close()


def prison(game, directory, corrections=False):
    for index, (delay, key) in enumerate(BOOT):
        game.step(delay)
        if index == 2:
            capture(game, directory, "00-sunlight")
        if index in (6, 7):
            capture(game, directory, "01-title" if index == 6 else "02-file")
        game.tap(key)
    wanted = {5596: "03-prison", 5599: "04-awake", 5601: "05-blood"}
    for _ in range(50):
        game.step(180)
        label = wanted.pop(game.resolver.dialogue_id, None)
        if label:
            game.step(140)  # Finish the native typewriter, including inserted names.
            capture(game, directory, label)
        if not wanted:
            if corrections:
                correction(game, directory)
            return
        game.tap("A")
    raise RuntimeError(f"Prison replay missed dialogue IDs {sorted(wanted)}")


def inventory(game, directory):
    route = ROOT / "tests/save_menus_field.txt"
    for raw in route.read_text().splitlines():
        words = raw.partition("#")[0].split()
        if not words:
            continue
        cmd, *args = words
        if cmd == "tap":
            game.tap(args[0])
        elif cmd == "wait":
            game.step(int(args[0]))
        elif cmd == "shot" and args[0] == "06_config":
            capture(game, directory, "06-config")
        elif cmd == "shot" and args[0] == "10_items":
            game.tap("A")
            game.step(55)
            capture(game, directory, "07-items")
            return
    raise RuntimeError("Inventory checkpoint missing from the stock replay")


def contact_sheet(directory):
    def font(size):
        try:
            return ImageFont.truetype("DejaVuSans.ttf", size)
        except OSError:
            return ImageFont.load_default(size=size)
    sheet = Image.new("RGB", (1008, 72 + 368 * len(ROWS)), "#16212b")
    pen = ImageDraw.Draw(sheet)
    for column, title in enumerate(("Japanese cartridge", "Live English")):
        pen.text((16 + column * 496, 20), title, font=font(28), fill="#ffffff")
    for index, (key, caption) in enumerate(ROWS.items()):
        y = 68 + index * 368
        pen.text((16, y), caption, font=font(19), fill="#b4cfde")
        for col, mode in enumerate(("japanese", "english")):
            with Image.open(directory / mode / (key + ".png")) as image:
                sheet.paste(image.resize((480, 320), Image.Resampling.NEAREST), (16 + 496 * col, y + 32))
    sheet.save(directory / "contact-sheet.png")


def run(memory):
    directory = OUT / "proof"
    directory.mkdir(exist_ok=True)
    with (directory / "transcript.jsonl").open("w") as transcript:
        for mode in ("japanese", "english"):
            dest = directory / mode
            dest.mkdir(exist_ok=True)
            for route, save in ((prison, prison_save()), (inventory, saves.TEMPLATE)):
                log_path = dest / (route.__name__ + ".jsonl")
                with log_path.open("w") as stream:
                    with session(memory, save, mode == "english", stream) as game:
                        if route is prison:
                            prison(game, dest, corrections=mode=="english")
                        else:
                            inventory(game, dest)
                for line in log_path.read_text().splitlines():
                    event = dict(run=mode, route=route.__name__, **json.loads(line))
                    transcript.write(json.dumps(event, ensure_ascii=False) + "\n")
            print(f"Captured {len(ROWS)} {mode} frames", flush=True)
    events = [json.loads(line) for line in (directory / "transcript.jsonl").read_text().splitlines()]
    keys = {e["key"] for e in events if e["run"] == "english" and e.get("english")}
    required = {"background/52AE/2/body", "graphic/731D/1", "graphic/530D/138", "text/5599", "text/5601", "text/1009", "text/1064"}
    if required - keys:
        raise RuntimeError(f"Missing proof encounters {sorted(required-keys)}")
    for key in ROWS:
        if (directory / "english" / (key+".png")).read_bytes() == (directory / "japanese" / (key+".png")).read_bytes():
            raise RuntimeError(f"Both renders identical at {key}")
    contact_sheet(directory)
    print(directory / "contact-sheet.png")
    print(directory / "transcript.jsonl")
