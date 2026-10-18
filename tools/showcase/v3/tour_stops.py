"""The stops of the Studio tour (tour.py): page, caption, interactions.

Numbers in the captions are checked against the repository when recording
(PROGRESS.md, docs/CUTSCENES.md, data/actor_registry.csv, docs/HANDOFF.md).
"""


def page(path, kicker, title, body, secs=8.0, ready=None, actions=None, where="", settle=1.5):
    def run(t, k, n, still=False):
        t.go(path, ready, settle=settle)
        t.caption(kicker, title, body, k, n, where or path)
        if still:
            t.hold(0)
            return
        if actions:
            actions(t)
        else:
            t.hold(secs)
    return {"title": title, "run": run}


def card(html, secs=6.0):
    def run(t, k, n, still=False):
        t.card(html)
        t.hold(0 if still else secs)
    return {"title": html[:40], "run": run}


def scroll_to(t, js_selector, secs=1.2, step=1 / 15, hold=4.0):
    """Smoothly scroll the Studio's main view to an element, then hold."""
    import time
    y = t.inner(f"(function(){{var e = {js_selector}; if (!e) return 0; var v = document.querySelector('#view') || "
                f"document.scrollingElement; return e.getBoundingClientRect().top + v.scrollTop - 80;}})()") or 0
    y0 = t.inner("(document.querySelector('#view') || document.scrollingElement).scrollTop") or 0
    n = max(1, int(secs / step))
    for i in range(n + 1):
        import math
        f = 0.5 - 0.5 * math.cos(math.pi * i / n)
        t.inner(f"(document.querySelector('#view') || document.scrollingElement).scrollTo(0, {y0 + (y - y0) * f:.0f})")
        t.shot(step)
    time.sleep(0.3)
    t.hold(hold)


def scroll_by(t, px, secs=1.5, hold=3.0):
    """Ease the Studio's scrolling view down by px, capturing as it moves, then hold."""
    import math
    view = "(document.querySelector('#view') || document.querySelector('main section:last-child') || document.scrollingElement)"
    y0 = t.inner(f"{view}.scrollTop") or 0
    n = max(1, int(secs * 15))
    for i in range(n + 1):
        f = 0.5 - 0.5 * math.cos(math.pi * i / n)
        t.inner(f"{view}.scrollTo(0, {y0 + px * f:.0f}); 1")
        t.shot(1 / 15)
    t.hold(hold)


def hover_path(t, points, secs=4.0):
    """Move the mouse through points (page coordinates) over secs, capturing."""
    n = int(secs * 10)
    for i in range(n):
        u = i / max(1, n - 1) * (len(points) - 1)
        k = min(len(points) - 2, int(u))
        f = u - k
        (x0, y0), (x1, y1) = points[k], points[k + 1]
        t.mouse(x0 + (x1 - x0) * f, y0 + (y1 - y0) * f)
        t.shot(0.1)


def play_song(t):
    t.hold(1.5)
    t.inner("$('play').click(); 1")
    t.hold(6.0, step=1 / 8)
    t.inner("$('stop').click(); 1")


TOOLS = """<h1>The <b>tools</b> behind it</h1><div class="grid">
<div><b>agbcc, instrumented</b>pret's GBA compiler with its register allocator opened up: <code>rasolve</code> solves an allocation inside the compiler, then names the C change that produces it.</div>
<div><b>decomp-permuter</b>A fork that searches C variations against agbcc, for the functions that resist by hand.</div>
<div><b>relatives.py</b>Finds matched functions that share long instruction runs with an unmatched one, and ports their C.</div>
<div><b>The harness and the shift test</b>A headless mGBA that runs the game from scripts, records coverage and video; every byte of data moves and the game still plays.</div>
<div><b>Centaur</b>mGBA linked to the Studio: subtitles, warps, captures, story state, and now live edits.</div>
<div><b>A GCL assembler</b>Writes the Metal Gear scripting language back into the ROM. Every scene in this video is one.</div>
</div>"""

CLOSING = """<h1>Boktai 3, <b>decompiled</b>: so far</h1><div class="grid">
<div><b>31.2% of the code</b>7,281 of 11,525 functions are C that compiles back to the exact bytes.</div>
<div><b>240 cutscenes, 632 scripts</b>Decoded, readable, and now writable.</div>
<div><b>496 scenes, 722 objects</b>Every room and every scriptable thing, mapped.</div>
<div><b>Thank you</b>The boktai3trans fan translation team, vgel and the Claudesona artists (N8python's CC0 sprites, Eidoverse), pret's agbcc, decomp-permuter, mGBA.</div>
</div>"""


STOPS = [
    card("<h1>How it was done: <b>the Studio</b></h1><p>Everything in that video came from reading the game. "
         "The Studio is where the reading happens: one place to browse, search and edit what the decompilation "
         "has learned about Boktai 3.</p>", secs=5.0),
    page("/atlas", "Browse", "One index of the <b>whole game</b>",
         ["The story, cast, monsters, places, cutscenes, objects, scripts, sprites, sound and code, "
          "all linked to each other.",
          "Every entry comes from the ROM itself, decoded by the decomp's tools."], secs=6.0),
    page("/atlas#node:Story", "Browse › Story", "Story progress is <b>one variable</b>",
         ["A single counter, 0 to 194, is the game's quest log. We mapped all 173 values it takes:",
          "where each is set, the cutscene around it, and the doors it opens."], secs=7.0),
    page("/atlas#cut:0", "Browse › Cutscenes", "Cutscenes are <b>data</b>",
         ["240 tables, 3,651 steps of camera, screen, sound and actor commands, played by one small director "
          "that is decompiled.",
          "Decoded first. Then written: every scene in the video is a new table."],
         actions=lambda t: (t.hold(3.0), scroll_by(t, 420, hold=4.0))),
    page("/atlas#char:9820", "Browse › Cast", "Every character, <b>every line</b>",
         ["Master Otenko's portrait comes from the game's own speaker table.",
          "All 337 of his lines, in script order, Japanese beside the fan translation."],
         actions=lambda t: (t.hold(3.0), scroll_by(t, 600, hold=3.5))),
    page("/atlas#actor:05A5", "Browse › Bestiary", "One sprite, <b>many palettes</b>",
         ["A monster family is one sprite drawn in different palette pools: the ghoul's C option picks pool 402 + C.",
          "The video's twenty ghouls are new pools appended to the same table."], secs=8.0),
    page("/actors", "Actors", "722 <b>script-callable</b> objects",
         ["The registry the scripts spawn from, named from Konami's own hashed identifiers.",
          "Each links to the code that runs it and the scenes that use it."], secs=6.0),
    page("/scripts#file=scene_6554", "Scripts", "GCL: the language of <b>Metal Gear Solid</b>",
         ["The scenes are scripts in GCL, Konami's scripting language, run by a virtual machine that is decompiled.",
          "Names recovered from MGS references and debug prints; shown as readable Python-like code."],
         actions=lambda t: (t.hold(3.5), scroll_by(t, 500, secs=2.0, hold=4.0))),
    page("/world", "World", "496 scenes, <b>76 areas</b>",
         ["Scenes grouped into areas by the doors between them.",
          "The doors are data too, which is how the video's doors could lie."], secs=7.0),
    page("/gfx#sprites/DE23", "Sprites", "Every sprite, in its <b>real colors</b>",
         ["Sprite packages and large sprites, drawn with the palettes the game really uses, traced through the code."],
         secs=7.0, ready="document.querySelectorAll('#view canvas').length > 3"),
    page("/gfx#pool/400", "Sprite palettes", "955 <b>palette pools</b>",
         ["The table every sprite takes its colors from: here, the ghoul's.",
          "The video appends its own entries after the game's."], secs=6.0,
         ready="document.querySelector('.palcard.on canvas')"),
    page("/gfx#maps/2C6E-CEBA-7E8B/2712.7", "Maps", "Stages, <b>rebuilt</b>",
         ["Map, tileset and terrain put back together, with doors, items and enemies where the scripts put them.",
          "This is the skylight room where the stranger was unmasked."], secs=8.0,
         ready="document.querySelectorAll('#view canvas, #view svg').length > 0", settle=4.0),
    page("/gfx#tiles/ts:CEBA", "Tiles", "<b>Tilesets</b>", ["Every tileset and the raw tile sheets behind maps and sprites."],
         secs=5.0, ready="document.querySelectorAll('#view canvas').length > 0"),
    page("/sound#18", "Sound", "Every song, <b>in the browser</b>",
         ["All 1,022 MP2K songs and effects, played from the ROM's own sequence data by a player written for the Studio."],
         actions=play_song),
    page("/#5460", "Text", "The script, <b>editable</b>",
         ["Every string, in the game font, Japanese and the fan translation side by side; edits build a new ROM."],
         secs=6.0),
    page("/rom", "ROM", "The whole <b>16 MB</b>",
         ["The ROM by what each byte is and how well we understand it."],
         actions=lambda t: (t.hold(2.0), hover_path(t, [(120, 300), (300, 420), (520, 380), (760, 450), (1000, 300)], 5.0))),
    page("/code#0805592C", "Code", "<b>31%</b> matching C",
         ["7,281 of 11,525 functions are C that compiles back to the original bytes, exactly.",
          "This one adds a keyframe sprite: every sprite the video drew went through it."], secs=8.0),
]

GAME_BOX = (1370, 400, 480, 320)     # where the game's recording goes, beside the Studio (edit.py)


def live(t, k, n, still=False):
    """Live edit: Centaur links a running game to the Studio. A line saved in
    the Text page and a color changed in Sprite palettes reach the game at
    once (centaur.lua settext/setpool). The game side is recorded by
    live_demo.lua and laid in beside the Studio by the edit."""
    import json
    import os
    import subprocess
    import time
    import urllib.request
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(os.path.dirname(os.path.dirname(here)))
    out = os.path.join(root, "build", "showcase", "v3", "live")
    t.top("document.getElementById('cap').innerHTML = %s" % json.dumps(
        "<div id='kicker'>Text, Sprite palettes, Live</div><div id='title'>Change the game <b>while it runs</b></div>"
        "<div id='body'><p>Centaur links mGBA to the Studio. Save a line or change a color, and the running game has it "
        "at once: Centaur writes it into the cartridge ROM in memory.</p></div><div id='gamebox' style='height:336px'></div>"
        "<div id='foot'><span id='dots'></span><span id='where'>/live</span></div>"))
    r = t.top("(function(){ var b = document.getElementById('gamebox').getBoundingClientRect(); "
              "return [Math.round(b.left), Math.round(b.top), 480, 320]; })()")
    t.meta["game_box"] = r
    if still:
        t.go("/#1587")
        t.hold(0)
        return
    if os.path.exists(out):
        import shutil
        shutil.move(out, os.path.expanduser(f"~/.Trash/live-{int(time.time())}"))
    os.makedirs(out)
    state = os.path.join(root, "build", "centaur", "scenarios", "panel", "panel_860.ss")
    rom = os.path.join(root, "build", "showcase", "v3", "plain.gba")
    # the line starts as the game has it (restored before any game is linked)
    orig = json.load(urllib.request.urlopen(t.url + "/api/original"))["1587"]
    urllib.request.urlopen(urllib.request.Request(t.url + "/api/string", data=json.dumps({"id": 1587, "text": orig}).encode(),
                                                  headers={"Content-Type": "application/json"}))
    env = dict(os.environ, LIVE_STATE=state, LIVE_OUT=out)
    game = subprocess.Popen([os.path.join(root, "tools", "mgba", "headless.sh"), os.path.join(here, "live_demo.lua"), rom],
                            env=env, stdout=open(os.path.join(out, "log.txt"), "w"), stderr=subprocess.STDOUT)

    def connected():
        try:
            return json.load(urllib.request.urlopen(t.url + "/api/live")).get("connected")
        except OSError:
            return False

    def until(cond, limit=120):
        for _ in range(int(limit * 4)):
            if cond():
                return True
            time.sleep(0.25)
        raise RuntimeError("live demo: timed out")
    until(connected)
    until(lambda: os.path.exists(os.path.join(out, "a_00769.png")))
    t.meta["live_start"] = t.now()
    t.go("/#1587")
    t.hold(3.5)
    new = "Hello from the Studio!\\nThis line was edited\\nwhile the game runs."
    t.inner("$('ed').value = ''; $('ed').dispatchEvent(new Event('input')); 1")
    shown = ""
    for ch in new.replace("\\\\n", "\\n"):
        shown += ch
        t.inner("$('ed').value = %s; $('ed').dispatchEvent(new Event('input')); 1" % json.dumps(shown))
        t.shot(1 / 20)
    t.hold(0.8)
    t.inner("$('save').click(); 1")
    t.meta["live_save"] = t.now()
    until(lambda: os.path.exists(os.path.join(out, "c_00769.png")))
    t.hold(9.0, step=1 / 4)
    t.go("/gfx#pool/42")
    t.hold(2.5)
    edits = [(15, "#2878f8"), (14, "#183898"), (15, "#e0745a"), (14, "#a04020")]
    t.meta["live_pool"] = []
    for i, (idx, hexc) in enumerate(edits):
        t.inner("spritePool().then(p => setPoolColor(p, 42, %d, %s, document.querySelectorAll('.palcard.on .pal i')[%d])); 1"
                % (idx, json.dumps(hexc), idx))
        t.meta["live_pool"].append(t.now())
        until(lambda: os.path.exists(os.path.join(out, f"d{i + 1}_00059.png")))
        t.hold(2.0, step=1 / 4)
    game.wait(timeout=60)
    t.meta["live_end"] = t.now()


STOPS.append({"title": "live", "run": live})
STOPS.append(card(TOOLS, secs=12.0))
STOPS.append(card(CLOSING, secs=9.0))
