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
        if actions and getattr(actions, "captions", False):     # it changes the caption as it goes
            actions(t, k, n)
        elif actions:
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
    """Scroll the map's room to the middle of the view, then switch the
    collision overlay on."""
    import time
    y = t.inner("(function(){var c=document.querySelector('#view canvas'); var v=document.querySelector('#view');"
                " var r=c.getBoundingClientRect(), vr=v.getBoundingClientRect();"
                " return Math.max(0, r.top - vr.top + v.scrollTop + r.height / 2 - v.clientHeight / 2)})()") or 0
    scroll_box(t, "document.querySelector('#view')", 0, y, secs=1.6, hold=3.0)
    t.inner("var cb=[...document.querySelectorAll('#view input[type=checkbox]')].find(c => "
            "c.parentElement.textContent.includes('terrain')); cb.click(); 1")
    time.sleep(1.5)
    t.hold(4.0)


GCL_PROC = 1939          # a scene script with dialogue and the developers' Japanese debug print


def gcl_bytes(pid):
    """The bytecode of script procedure `pid` as hex lines (ROM-derived: shown, never stored)."""
    import os
    import sys
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(os.path.dirname(os.path.dirname(here)))
    sys.path[:0] = [os.path.join(root, "tools"), os.path.join(root, "tools", "assets")]
    import gcx
    from romlib import load_rom
    rom = load_rom()
    img = gcx.Image(rom)
    o = img.proc_address(pid - 1)
    st, n = gcx.length(bytes(rom), o)
    b = bytes(rom[o:st + n])
    return [" ".join(f"{x:02X}" for x in b[i:i + 16]) for i in range(0, min(len(b), 16 * 26), 16)]


def gcl_steps(t, k, n):
    """Three views of one script: its bytecode, the GCL it decompiles to
    (Konami-style, as the compiler laid it out, Japanese and all), and our
    readable version. The caption changes with each."""
    import json
    rows = gcl_bytes(GCL_PROC)
    t.inner("(function(){ var b = document.getElementById('body'); if (!b) return 0;"
            " b.scrollIntoView({block: 'start'}); document.getElementById('view').scrollBy(0, -60);"
            " b.dataset.keep = b.textContent; b.textContent = %s;"
            " b.previousElementSibling.textContent = 'Bytecode, as it is stored in the ROM.'; return 1; })()"
            % json.dumps("\n".join(rows)))
    t.hold(6.5)
    t.caption("Scripts", "GCL, as Konami <b>wrote it</b>",
              ["We decompiled the virtual machine, then the bytecode itself: back into GCL as Konami's compiler "
               "laid it out, down to the developers' debug messages in Japanese."], k, n, "/scripts")
    t.inner("(function(){ var c = document.getElementById('readable'); if (c.checked) c.click(); return 1; })()")
    import time
    time.sleep(1.2)
    t.inner("document.getElementById('body').scrollIntoView({block: 'start'}); document.getElementById('view').scrollBy(0, -60); 1")
    t.hold(6.5)
    t.caption("Scripts", "...and as you can <b>read it</b>",
              ["Then into a readable form, with the names we recovered for procedures, variables and commands. "
               "Our assembler writes it all back, byte for byte."], k, n, "/scripts")
    t.inner("(function(){ var c = document.getElementById('readable'); if (!c.checked) c.click(); return 1; })()")
    time.sleep(1.2)
    t.inner("document.getElementById('body').scrollIntoView({block: 'start'}); document.getElementById('view').scrollBy(0, -60); 1")
    t.hold(6.5)


gcl_steps.captions = True


def cutscene_beats(t, k, n):
    t.hold(5.5)
    t.caption("Browse > Cutscenes", "...and the tool that <b>reads them</b>",
              ["The Studio decodes EVERY cutscene table into a timeline you can read and edit. "
               "The scenes in this video were written the same way."], k, n, "/atlas#cut:0")
    scroll_by(t, 420, hold=4.5)


cutscene_beats.captions = True


TOOLS = gcard("The <b>tools</b> behind it", [
    ("agbcc, built for agents", "pret's GBA compiler, instrumented to explain its register allocation, so an agent can see which C change gives the right registers."),
    ("A faster permuter", "decomp-permuter reworked to search faster and waste less, so agents can run it on every stubborn function."),
    ("Centaur", "Our mGBA fork with a Lua harness that humans and agents share, talking to the Studio in real time."),
    ("A headless harness", "Plays the game from scripts and records coverage, screenshots and video, many copies at once."),
    ("A GCL assembler", "Writes Konami's scripting language back into the ROM, byte for byte."),
    ("Dozens of Python tools", "Pointers, relocations, assets, coverage, ROM maps: the tooling that keeps EVERY byte accounted for."),
])

CLOSING = gcard("Boktai 3, <b>decompiled</b>", [
    ("41% matching C", "C that compiles back to the exact original bytes, and growing. Contributions welcome!"),
    ("EVERY script and cutscene", "Decoded, readable, and writable."),
    ("EVERY room and object", "Mapped, linked, and editable in the Studio."),
    ("All you need is the ROM", "The decomp and the Studio are open source."),
], foot="<b>github.com/moozilla/boktai3-decomp</b>   Contributions welcome!")


def big_card(lines, secs):
    """A card of statements in the game's font: (text, scale) lines."""
    html = "".join(f'<div class="gf" data-scale="{sc}" data-text="{x}"></div><div class="gap"></div>' for x, sc in lines)
    return card(html, secs=secs)


STOPS = [
    big_card([("Every scene you just watched was written <b>directly into the ROM</b>", 4),
              ("and played by the game's own cutscene engine.", 4),
              ("<m>This is just a taste of what a full decompilation makes possible.</m>", 3)], 7.0),
    big_card([("...and that's <b>not all</b>.", 5),
              ("We're also releasing the <b>Boktai 3 Studio</b>: a complete toolkit", 3),
              ("to explore and edit EVERY piece of data in the game.", 3)], 6.5),
    page("/atlas", "Browse", "The whole game, <b>decoded</b>",
         ["EVERY script, cutscene, line of text, object, map, sprite and song, decoded and linked together:",
          "5,615 scripts, 240 cutscenes, 9,977 lines of text, 722 object types, 287 maps, 1,022 songs and sounds."],
         secs=8.0),
    page("/code#0805592C", "Code", "<b>41%</b> matching C, and growing",
         ["The game's code is ARM assembly. We're rewriting it as C that compiles back to the exact same bytes. "
          "Contributions welcome!",
          "This function adds a keyframe sprite: everything the stranger did on screen went through it."],
         secs=8.0),
    page(f"/scripts#{GCL_PROC}", "Scripts", "A second language, <b>decompiled</b>",
         ["Boktai's story, scenes and events aren't in the C code at all. They're bytecode for GCL, the scripting "
          "language Konami built for the Metal Gear Solid engine and adapted for the GBA, run by its own virtual "
          "machine inside the game. So we decompiled that too."],
         actions=gcl_steps, ready="document.getElementById('body') && document.getElementById('body').textContent.length > 50",
         settle=2.0),
    page("/atlas#cut:0", "Browse > Cutscenes", "How the cutscene <b>engine</b> works",
         ["A cutscene is a table of steps. Each step sends messages to receivers: the camera, the screen, the music, "
          "Django, Otenko, Vanargand. Then it waits: for some frames, a camera move, or the player to close a message."],
         actions=cutscene_beats),
    page("/#5460", "Text", "EVERY line, in <b>both languages</b>",
         ["The Japanese script and the 2007 fan translation side by side, previewed in the game's own font, and editable."],
         secs=6.5),
    page("/actors", "Actors", "EVERY object, <b>named</b>",
         ["Scripts spawn objects by hashed name. We recovered Konami's names and linked EVERY object to the code "
          "that runs it."], secs=6.5),
    page("/world", "World", "Rooms, connected by their <b>doors</b>",
         ["Rooms connect through door data. The Studio follows the doors to group EVERY scene into its area."],
         secs=6.0),
    page("/gfx#dungeon/3", "Dungeons", "Whole areas, <b>stitched</b>",
         ["...and lays out whole areas room by room, drawn from the game's own map data."],
         actions=dungeon, ready="document.querySelectorAll('#dbox canvas').length > 20", settle=3.0),
    page("/gfx#maps/2C6E-CEBA-7E8B/2712.7", "Maps", "EVERY stage, <b>rebuilt</b>",
         ["Map, tileset and the collision layer underneath, with every door, item and enemy where the scripts "
          "put them. This is the skylight room."],
         actions=map_terrain, ready="document.querySelectorAll('#view canvas').length > 0", settle=4.0),
    page("/gfx#sprites/DE23", "Sprites", "EVERY sprite in its <b>real colors</b>",
         ["Sprites don't carry their own palettes: the code picks one at run time. We traced the palettes through "
          "the code, so EVERY sprite shows in its real colors."],
         secs=7.0, ready="document.querySelectorAll('#view canvas').length > 3"),
    page("/sound#18", "Sound", "EVERY song, <b>in your browser</b>",
         ["The music is sequence data for the GBA's sound engine. The Studio's own player synthesizes EVERY song "
          "in the browser, track by track."],
         actions=play_song),
    page("/rom", "ROM", "The whole ROM, <b>mapped</b>",
         ["EVERY byte, classified: code, data, text, graphics or sound."],
         actions=lambda t: (t.hold(2.0), hover_path(t, [(120, 300), (300, 420), (520, 380), (760, 450), (1000, 300)], 5.0))),
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
    t.caption("Text, Live", "Change the game <b>while it runs</b>",
              ["Edit a line of dialogue in the Studio and save it: the running game on the right shows the new line "
               "at once."],
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
    t.caption("Sprite palettes, Live", "Recolor a sprite <b>live</b>",
              ["This is Django's palette. Change a color and his scarf changes in the running game: "
               "blue, then Claude's orange."],
              k, n, "/live")
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


def centaur(t, k, n, still=False):
    """Centaur: the Studio's Live page following a running game (live_tour.lua
    walks Django around the skylight room at the GBA's frame rate and saves
    its frames for the edit to lay in beside the Studio). The screen
    inspector captures it every two seconds; a sprite is picked; the
    palettes and BG layers tabs; the map with the player on it."""
    import json
    import os
    import subprocess
    import time
    import urllib.request
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(os.path.dirname(os.path.dirname(here)))
    out = os.path.join(root, "build", "showcase", "v3", "livetour")
    t.caption("Live", "Centaur: the Studio, linked to <b>a running game</b>",
              ["Centaur is our fork of mGBA, with a Lua harness that humans and agents share. The Live page follows "
               "the game as it plays: the scene, the story state, the objects on screen."], k, n, "/live")
    t.top("document.getElementById('gamebox').style.height = '336px'; 1")
    t.meta["game_box"] = t.top("(function(){ var b = document.getElementById('gamebox').getBoundingClientRect(); "
                               "return [Math.round(b.left), Math.round(b.top), 480, 320]; })()")
    if still:
        t.go("/live")
        t.hold(0)
        return
    os.makedirs(out, exist_ok=True)          # reused: frames are overwritten
    state = os.path.join(root, "build", "centaur", "scenarios", "panel", "panel_860.ss")
    rom = os.path.join(root, "build", "showcase", "v3", "plain.gba")
    env = dict(os.environ, LIVE_STATE=state, LIVE_OUT=out, LIVE_FRAMES="2700")
    game = subprocess.Popen([os.path.join(root, "tools", "mgba", "headless.sh"), os.path.join(here, "live_tour.lua"), rom],
                            env=env, stdout=open(os.path.join(out, "log.txt"), "w"), stderr=subprocess.STDOUT)

    def connected():
        try:
            return json.load(urllib.request.urlopen(t.url + "/api/live")).get("connected")
        except OSError:
            return False
    for _ in range(240):
        if connected() and os.path.exists(os.path.join(out, "g_00010.png")):
            break
        time.sleep(0.25)
    t.meta["game_start"] = t.now() - 10 / 59.7275
    t.go("/live")
    t.hold(4.0, step=1 / 4)
    # the screen inspector, following the game
    t.inner("document.getElementById('screenview').closest('.card').scrollIntoView({block: 'start'}); 1")
    for _ in range(3):                      # the inspector captures the running game (its Refresh)
        t.inner("snap(); 1")
        t.hold(1.75, step=1 / 4)
    t.caption("Live", "Capture <b>anything</b>",
              ["A capture maps EVERY sprite, tile and palette on screen back to its data. Click a sprite and the "
               "Studio tells you what it is, which frame, and which palette."], k, n, "/live")
    t.inner("snap(); 1")
    t.hold(1.5, step=1 / 4)
    t.inner("(function(){ var obs = [...document.querySelectorAll('#screenview .screen .ob')];"
            " obs.sort((a, b) => b.offsetWidth * b.offsetHeight - a.offsetWidth * a.offsetHeight);"
            " if (obs[0]) obs[0].click(); return obs.length; })()")
    t.hold(4.0, step=1 / 4)
    for tab in ("palettes", "layers"):
        t.inner("(function(){ var b = document.querySelector('#screenview .tabs button[data-t=%s]'); if (b) b.click();"
                " document.querySelector('#screenview .tabs').scrollIntoView({block: 'start'}); return 1; })()" % json.dumps(tab))
        t.hold(3.0, step=1 / 4)
    t.caption("Live", "...and follow it on <b>the map</b>",
              ["The same link puts the player on the stage map, with the doors, items and enemies around them."],
              k, n, "/live")
    t.inner("document.getElementById('livemap').scrollIntoView({block: 'start'}); 1")
    t.hold(5.0, step=1 / 4)
    t.meta["game_end"] = t.now()
    game.terminate()


STOPS.append({"title": "centaur", "run": centaur})
STOPS.append({"title": "live", "run": live})
STOPS.append(card(TOOLS, secs=12.0))
STOPS.append(card(CLOSING, secs=9.0))
