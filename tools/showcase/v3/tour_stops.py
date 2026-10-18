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


def gcard(title, items=None, text=None, foot=None):
    """A full-frame card in the game's font: a title, then either a grid of
    (heading, text) items or a paragraph, and an optional footer line."""
    def esc(x):
        return x.replace('"', "&quot;")
    html = f'<div class="gf" data-scale="5" data-text="{esc(title)}"></div><div class="gap"></div>'
    if items:
        html += '<div class="grid">' + "".join(
            f'<div><div class="gf" data-scale="3" data-text="{esc(h)}"></div><div style="height:10px"></div>'
            f'<div class="gf" data-scale="2" data-text="<m>{esc(b)}</m>"></div></div>' for h, b in items) + '</div>'
    if text:
        html += f'<div class="gf" data-scale="3" data-text="{esc(text)}"></div>'
    if foot:
        html += f'<div class="gap"></div><div class="gf" data-scale="3" data-text="{esc(foot)}"></div>'
    return html


def scroll_box(t, js, dx, dy, secs=4.0, hold=1.5):
    """Ease an element's scroll position by (dx, dy), capturing as it moves."""
    import math
    x0 = t.inner(f"({js}).scrollLeft") or 0
    y0 = t.inner(f"({js}).scrollTop") or 0
    n = max(1, int(secs * 15))
    for i in range(n + 1):
        f = 0.5 - 0.5 * math.cos(math.pi * i / n)
        t.inner(f"({js}).scrollTo({x0 + dx * f:.0f}, {y0 + dy * f:.0f}); 1")
        t.shot(1 / 15)
    t.hold(hold)


def dungeon(t):
    """The stitched area: first the whole of it, fitted to the view, then a
    slow zoom into the middle of its rooms. The layout is one big element
    the page scales with a transform; the tour sets that transform itself."""
    import math
    import time
    box = t.inner("""(function(){ var b = document.getElementById('dbox'), inner = b.firstElementChild;
        var x0 = 1e9, y0 = 1e9, x1 = -1e9, y1 = -1e9, xs = [], ys = [];
        inner.querySelectorAll('canvas').forEach(c => { var l = parseFloat(c.style.left), tp = parseFloat(c.style.top);
            x0 = Math.min(x0, l); y0 = Math.min(y0, tp); x1 = Math.max(x1, l + c.width); y1 = Math.max(y1, tp + c.height);
            xs.push(l + c.width / 2); ys.push(tp + c.height / 2); });
        xs.sort((a, b) => a - b); ys.sort((a, b) => a - b);
        b.style.overflow = 'hidden'; b.scrollTo(0, 0); inner.style.transformOrigin = '0 0';
        return [x0, y0, x1, y1, xs[xs.length >> 1], ys[ys.length >> 1], b.clientWidth, b.clientHeight]; })()""")
    x0, y0, x1, y1, mx, my, vw, vh = box
    fit = min(vw / (x1 - x0), vh / (y1 - y0)) * 0.96
    fx = (vw - (x1 - x0) * fit) / 2 - x0 * fit
    fy = (vh - (y1 - y0) * fit) / 2 - y0 * fit

    def place(k, tx, ty):
        t.inner(f"document.getElementById('dbox').firstElementChild.style.transform = "
                f"'translate({tx:.1f}px, {ty:.1f}px) scale({k:.4f})'; 1")
    place(fit, fx, fy)
    time.sleep(1.0)
    t.hold(3.0)
    end_k = 0.42
    tx, ty = vw / 2 - mx * end_k, vh / 2 - my * end_k
    n = 6 * 15
    for i in range(n + 1):
        f = 0.5 - 0.5 * math.cos(math.pi * i / n)
        k = math.exp(math.log(fit) + (math.log(end_k) - math.log(fit)) * f)     # zoom evenly
        g = (k - fit) / (end_k - fit)
        place(k, fx + (tx - fx) * g, fy + (ty - fy) * g)
        t.shot(1 / 15)
    t.hold(2.0)


def map_terrain(t):
    """Scroll down to the map, then switch the collision overlay on."""
    import time
    y = t.inner("(function(){var c=document.querySelector('#view canvas'); var v=document.querySelector('#view');"
                " return c.getBoundingClientRect().top + v.scrollTop - 110})()") or 0
    scroll_box(t, "document.querySelector('#view')", 0, y, secs=1.6, hold=3.0)
    t.inner("var cb=[...document.querySelectorAll('#view input[type=checkbox]')].find(c => "
            "c.parentElement.textContent.includes('terrain')); cb.click(); 1")
    time.sleep(1.5)
    t.hold(4.0)


TOOLS = gcard("The <b>tools</b> behind it", [
    ("agbcc, opened up", "pret's GBA compiler with its register allocator instrumented: solve the allocation, then name the C change that produces it."),
    ("A permuter", "Searches C variations against the compiler for the functions that resist by hand."),
    ("relatives.py", "Finds matched functions that share long runs with an unmatched one, and ports their C."),
    ("A headless harness", "Plays the game from scripts, records coverage and video. Every byte of data can move and the game still plays."),
    ("Centaur", "mGBA linked to the Studio: subtitles, warps, captures, story state, live edits."),
    ("A GCL assembler", "Writes Konami's scripting language back into the ROM. Every scene in this video is one."),
])

CLOSING = gcard("Boktai 3, <b>decompiled</b>", [
    ("31% of the code", "C that compiles back to the exact original bytes, and climbing."),
    ("Every script and cutscene", "Decoded, readable, and writable."),
    ("Every room and object", "Mapped, linked, and editable in the Studio."),
    ("All you need is the ROM", "The decomp and the Studio are open source."),
], foot="<b>github.com/moozilla/boktai3-decomp</b>   Contributions welcome!")


STOPS = [
    card(gcard("How it was <b>done</b>", text="Everything you just saw was written into the game by reading it first. "
                                              "This is where the reading happens: the Boktai 3 Studio, in your browser.")),
    page("/atlas", "Browse", "The whole game, <b>understood</b>",
         ["Not just the code: the story, the cast, every monster, room, cutscene, script, sprite and song, "
          "decoded from the ROM and linked together."], secs=6.0),
    page("/atlas#node:Story", "Browse > Story", "Every <b>beat</b> of the story",
         ["The game keeps its whole plot in one number. We know every value it takes, where it is set, "
          "and which doors it opens."], secs=6.5),
    page("/atlas#cut:0", "Browse > Cutscenes", "Cutscenes are <b>data</b>",
         ["Every camera move, line and sound is a table played by a tiny director we decompiled.",
          "So we wrote our own. Every scene in this video is one."],
         actions=lambda t: (t.hold(2.5), scroll_by(t, 420, hold=3.5))),
    page("/scripts#file=scene_6554", "Scripts", "The code <b>inside</b> the code",
         ["Boktai 3's scenes are written in GCL, the scripting language Konami built for Metal Gear Solid, "
          "running on its own virtual machine.",
          "We decompiled the machine, then its bytecode, back into readable scripts."],
         actions=lambda t: (t.hold(3.5), scroll_by(t, 500, secs=2.0, hold=4.0))),
    page("/atlas#char:9820", "Browse > Cast", "Every character, <b>every line</b>",
         ["Portraits from the game's own speaker table, and everything each character says, "
          "beside the fan translation."],
         actions=lambda t: (t.hold(2.5), scroll_by(t, 600, hold=3.0))),
    page("/atlas#actor:05A5", "Browse > Bestiary", "One ghoul, <b>many colors</b>",
         ["A single option in a monster's script picks its palette.",
          "The twenty ghouls in the cold open were twenty new palettes."], secs=7.0),
    page("/actors", "Actors", "Everything a script can <b>spawn</b>",
         ["Doors, switches, enemies, Vanargand: named from Konami's own hashed identifiers and linked "
          "to the code that runs them."], secs=6.0),
    page("/world", "World", "Every room, and how they <b>connect</b>",
         ["Scenes grouped into areas by their doors. The doors are data too, which is how they could lie."],
         secs=6.0),
    page("/gfx#dungeon/3", "Dungeons", "Whole places, <b>stitched</b>",
         ["Every room of Lifeless Town laid out by its doors, rebuilt from the game's own map data."],
         actions=dungeon, ready="document.querySelectorAll('#dbox canvas').length > 20", settle=3.0),
    page("/gfx#maps/2C6E-CEBA-7E8B/2712.7", "Maps", "Stages, <b>rebuilt</b>",
         ["Map, tileset and the collision underneath, with every door, item and enemy where the scripts put them.",
          "This is the skylight room."],
         actions=map_terrain, ready="document.querySelectorAll('#view canvas').length > 0", settle=4.0),
    page("/gfx#sprites/DE23", "Sprites", "Every sprite, in its <b>real colors</b>",
         ["Palettes traced through the code to the sprites that really use them."],
         secs=6.0, ready="document.querySelectorAll('#view canvas').length > 3"),
    page("/sound#18", "Sound", "Every song, <b>in your browser</b>",
         ["A player built for the Studio plays the game's own sequence data, track by track."],
         actions=play_song),
    page("/#5460", "Text", "Every line, <b>editable</b>",
         ["The Japanese script and the fan translation side by side, previewed in the game's own font."],
         secs=6.0),
    page("/rom", "ROM", "The whole ROM, <b>mapped</b>",
         ["Every byte: what it is, and how well we understand it."],
         actions=lambda t: (t.hold(2.0), hover_path(t, [(120, 300), (300, 420), (520, 380), (760, 450), (1000, 300)], 5.0))),
    page("/code#0805592C", "Code", "<b>31%</b> matching C",
         ["C that compiles back to the exact original bytes.",
          "This function adds a keyframe sprite: everything the stranger did on screen went through it."],
         secs=7.0),
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
    t.caption("Text, Sprite palettes, Live", "Change the game <b>while it runs</b>",
              ["Supply the ROM, open the Studio, and edit anything. A linked emulator gets the change instantly."],
              k, n, "/live")
    t.top("document.getElementById('gamebox').style.height = '336px'; 1")
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
    until(lambda: os.path.exists(os.path.join(out, "a_00639.png")))
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
    until(lambda: os.path.exists(os.path.join(out, "c_00699.png")))
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
