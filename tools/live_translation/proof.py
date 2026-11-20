"""Replay semantic checkpoints twice, retaining raw GBA frames and encounter evidence."""
from contextlib import closing
from hashlib import sha256
from pathlib import Path
import json

from PIL import Image, ImageChops, ImageDraw, ImageFont
from player import BOOT
from translator import Memory

from frames import ArtTranslator
from localize import Localizer
from runtime import Game, OUT, ROOT, baserom_path, checkpoint, prison_save, saves

ROWS = {
    "00-sunlight": "Cartridge sunlight advisory / baked background",
    "01-title": "Title logo and subtitle",
    "02-file": "File selection and button legends",
    "03-prison": "Sealed Prison / first voice",
    "04-awake": "Otenko speaks the saved name",
    "05-blood": "Dialogue fitted to the message window",
    "06-config": "System settings",
    "07-items": "Item name and scrolling description",
    "08-puppeteer": "Sabata's dream / new game",
    "09-title-card": "Prologue title card",
    "10-tree": "Ancient Tree / first visit",
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


def tree(game, directory):
    for delay, key in BOOT:
        game.step(delay)
        game.tap(key)
    for _ in range(40):
        game.step(165)
        if game.resolver.dialogue_id == 6105:
            game.step(140)
            capture(game, directory, "10-tree")
            return
        game.tap("A")
    raise RuntimeError("Did not reach the Ancient Tree conversation")


def prologue(game, directory):
    """Match script IDs, since English and Japanese use different numbers of frames."""
    wanted = {5492: "08-puppeteer", 2171: "09-title-card"}
    commands = (ROOT/"tests/prologue.txt").read_text().splitlines()
    for raw in commands + ["tap A", "wait 50"]*120:
        words = raw.partition("#")[0].split()
        if not words:
            continue
        if words[0] == "wait":
            game.step(int(words[1]))
        elif words[0] == "tap":
            game.tap(words[1])
        ident = game.resolver.dialogue_id
        if ident in wanted:
            game.step(280 if ident == 5492 else 75)
            capture(game, directory, wanted.pop(ident))
        if not wanted:
            return
    raise RuntimeError(f"Prologue did not reach {sorted(wanted)}")


def title_correction(memory, directory, transcript, save):
    private = Memory(":memory:")
    memory.db.backup(private.db)
    where = directory / "english/logo-correction"
    where.mkdir(exist_ok=True)
    key = "graphic/731D/1"
    path = where / "encounters.jsonl"
    try:
        with path.open("w") as log, session(private, save, True, log) as game:
            loc = game.resolver
            loc.inspect_key = key
            for n, button in BOOT[:6]:
                game.step(n)
                game.tap(button)
            def take(label):
                game.rewind_line(1)
                game.step(300)
                image = game.image()
                image.save(where/(label+".png"))
                return image
            first = take("01-english")
            loc.original_key = key
            original = take("02-japanese-wordmark")
            loc.original_key = None
            old = private.db.execute("SELECT english FROM entries WHERE key=?", (key,)).fetchone()["english"]
            new = "Solar Boy" if old != "Solar Boy" else "Boktai"
            private.correct(key,new,author="scripted proof correction")
            edited = take("03-corrected")
            for image in (original, edited):
                bounds = ImageChops.difference(first,image).getbbox()
                if not bounds or bounds[3] > 97:
                    raise RuntimeError(f"Unexpected change outside title lettering: {bounds}")
            from editor import render
            view = render(edited,loc.inspected,new,len(new),False,"scripted proof correction","")
            Image.frombytes("RGBA",(900,600),view).save(where/"04-editor.png")
            # Leave the gallery's most recent preview on the owner's wording.
            private.correct(key,old,author="proof restore")
            game.rewind_line(1)
            game.step(2)
        for line in path.read_text().splitlines():
            event = dict(run="english", route="logo-correction", **json.loads(line))
            transcript.write(json.dumps(event,ensure_ascii=False)+"\n")
    finally:
        private.db.close()


def edit_sheet(directory):
    sheet = Image.new("RGB",(1488,780),"#16212b")
    pen = ImageDraw.Draw(sheet)
    for i,title in enumerate(("Source","Live English","After correction")):
        pen.text((12+492*i,12),title,font=ImageFont.load_default(size=23),fill="white")
    for row,(folder,source) in enumerate((("logo-correction","02-japanese-wordmark"),("correction","02-japanese"))):
        for col,name in enumerate((source,"01-english","03-corrected")):
            with Image.open(directory/"english"/folder/(name+".png")) as im:
                sheet.paste(im.resize((480,320),Image.Resampling.NEAREST),(12+492*col,50+row*365))
    sheet.save(directory/"edits.png")


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
    opening_save = prison_save()
    routes = [(prison, opening_save), (inventory, saves.TEMPLATE), (prologue, None),
              (tree, checkpoint(19, 3363, 66, 2))]
    inputs = {"cartridge": Path(baserom_path())}
    inputs.update({route.__name__ + "_save": Path(save) for route, save in routes if save})
    def input_hashes():
        return {role: sha256(path.read_bytes()).hexdigest() for role, path in inputs.items()}
    original = input_hashes()
    with (directory / "transcript.jsonl").open("w") as transcript:
        for mode in ("japanese", "english"):
            dest = directory / mode
            dest.mkdir(exist_ok=True)
            for route, save in routes:
                log_path = dest / (route.__name__ + ".jsonl")
                with log_path.open("w") as stream:
                    with session(memory, save, mode == "english", stream) as game:
                        if route is prison:
                            prison(game, dest, corrections=mode=="english")
                        else:
                            route(game, dest)
                for line in log_path.read_text().splitlines():
                    event = dict(run=mode, route=route.__name__, **json.loads(line))
                    transcript.write(json.dumps(event, ensure_ascii=False) + "\n")
            print(f"Captured {len(ROWS)} {mode} frames", flush=True)
        title_correction(memory, directory, transcript, opening_save)
    if input_hashes() != original:
        raise RuntimeError("The cartridge or an input save changed during the proof")
    events = [json.loads(line) for line in (directory / "transcript.jsonl").read_text().splitlines()]
    keys = {e["key"] for e in events if e["run"] == "english" and e.get("english")}
    required = {"background/52AE/2/body", "graphic/731D/1", "graphic/530D/138", "text/5599", "text/5601", "text/1009", "text/1064", "text/5492", "text/2171", "text/6105", "graphic/530D/131/swap"}
    if required - keys:
        raise RuntimeError(f"Missing proof encounters {sorted(required-keys)}")
    for key in ROWS:
        if (directory / "english" / (key+".png")).read_bytes() == (directory / "japanese" / (key+".png")).read_bytes():
            raise RuntimeError(f"Both renders identical at {key}")
    contact_sheet(directory)
    edit_sheet(directory)
    from collections import Counter
    summary = {
        "paired_checkpoints": len(ROWS),
        "rom_sha256": next(e["rom_sha256"] for e in events if "rom_sha256" in e),
        "english_encounters": dict(Counter(e["event"] for e in events if e["run"] == "english")),
        "text_and_art_choices": len(keys),
        "routes": sorted({e["route"] for e in events}),
        "sources": sorted({e["provenance"] for e in events if e.get("provenance")}),
        "verified_unchanged_sha256": original,
    }
    (directory/"summary.json").write_text(json.dumps(summary, indent=2)+"\n")
    print((directory / "contact-sheet.png").relative_to(ROOT))
    print((directory / "transcript.jsonl").relative_to(ROOT))
