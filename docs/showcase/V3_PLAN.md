# Showcase v3: "The Stranger" (plan and script)

Tags: #showcase #scripts #tooling

The brief for the third showcase video. v1 and v2 are in
`~/Movies/boktai3-showcase/`; their builders are `tools/showcase/showcase_v1.py`
and `showcase.py` (v2). This one aims as high as it can: a long cut
(8–12 minutes) that we cut down later. Two parts:

1. **The hack (6–8 min).** A cloaked stranger does impossible things to
   the game (one ghoul fans out into twenty, doors lead everywhere, bosses
   forget their fights) while Django and Otenko chase it across the world.
   At the end Django's gun overheats, the SUN gauge explodes, and the
   stranger is revealed as Claude, who decompiled the game. Then an encore
   (a train of boss coffins) and the credits. Everything is played by the
   game's engine: new scripts, cutscene tables, sprites, palettes, stage
   data.
2. **How it was done (4–6 min).** The Studio and the tooling, page by page,
   each with a caption panel saying what is interesting about it.

Everything ROM-derived (patched ROMs, frames, videos) stays under `build/`
and `~/Movies/boktai3-showcase/` (v3 next to v1 and v2). Tools go in
`tools/showcase/` and get a PR. Read `docs/CUTSCENES.md` (the table format,
receivers, commands) and the v2 builder before starting; they record what
works and what was learned the hard way (below).

## What we already have

* `gclasm.py`: a GCL assembler (commands with options, calls, `switch`,
  `if`/`else`, assignments), byte-identical to the compiler for those forms.
* `rom.py`: procedures in unused slots, strings on borrowed ids, cutscene
  tables in event slots, patrol paths added to a stage's terrain, palette
  pools, sprite packages, archive entries, portrait frames.
* Sprite packages drawn with ScriptKeyframe: `EAB4` add (n tag, d package, i
  frame, p x y z), `44CD` set frame (n, p), `997B` glide to (n, p x y z,
  f frames), `EBFF` hide, `9C3F` show, `ED1F` mirror. **A package's first
  header word is its palette pool entry**; part palette bits are ignored.
* Enemies: a spawner's `r=(path, 0, 0)` puts it at the first point of that
  patrol path; `n=(0, 0)` keeps it still; `C` picks its palette (pool 402+C
  for the ghoul family `Bok`).
* Portraits: `593A`/`593B` frames (32×32, one part, 16 tiles), palette pool
  per speaker id (`atlas.speaker_portraits`); v2 took over GARIGARIKUN and
  ROCKMAN.
* Cutscene receivers: the director (run, wait, wait for signal), the player
  (`2B06` in prologue-style scenes; stand, face, walk_to, pose...), Otenko as
  `character 1` (`9CFE`, kind 5), Vanargand (`7C56`, kind 4: 51/52/56/62 are
  its rise), camera (`snap_to`, `pan_to`), screen (`fade` with a color and a
  strength, `quake`, `particles_at`), audio (sounds, BGM).
* The dialogue procedure idiom: each "run" shows one message and its own
  `message_then` closes it; one message can hold several pages and speakers.
* `video.sh` (scenario savestate, then the harness records H.264/AAC with
  mGBA's encoder), `studio_tour.py` (headless Chrome over the DevTools
  protocol), `edit.sh` (cards and concatenation).

Gotchas already paid for: the dream stage's floor appears only once
Vanargand is spawned (its boss layer sets the layers up); `chara`/`command`
spawns in a room need the room's resources, so test new objects in the scene
they will appear in; harness savestates must be taken after a warp has
finished; the sprite package tile sheet must be 16 tiles wide.

## Part 1: the hack

Rules from the user's reviews:

* **Hook in the first 5–10 seconds** with something visual and impossible.
  No slow prologue.
* **Claude stays hidden until the end of Part 1.** Through most of the
  video there is only a **stranger**: a cloaked figure at the edge of the
  frame, face hidden, who does impossible things to the game and is gone
  before anyone catches up. Django and Otenko chase it: "Who *is* that?"
  Viewers who don't know or care about the Claudesona get a mystery, not a
  mascot. The reveal comes from the sun gauge (below).
* **Show, don't tell.** Every set piece is visual and ends on a punchline.
  Editing dialogue is not a set piece: everyone knows text can be hacked.
  A character talks only when they have just appeared somewhere impossible.
* **Funny.** Django is deadpan and put upon; Otenko is scandalized; the
  stranger never explains itself.
* **Not beat for beat.** The list below is a menu, not a running order.
  Prove the mechanisms, keep the best 6–8, and order them for escalation and
  variety (big, small, big; fast cuts between places). Invent better ones
  where the engine allows; the bar is "I didn't know the game could do that".
* **No glitch noise and no Clawd.** Every effect is a capability of the
  decompiled engine. The art effort goes into the stranger and the
  Claudesona, both of which must look like original Boktai art.
* **Travel.** Real places from across the game (save checkpoints from
  `saves.py` give the right story state), with their own music.

Dialogue pacing: faster than v2 (which pressed A every 105 frames). Use
adaptive timing, `frames = 30 + 1.8 * characters on the page`; the recorder
presses A per page on that schedule. Most lines are one page.

### Cold open (0:00–0:10)

Django walks up a corridor. A ghoul shambles toward him. Behind it, at the
far end, the stranger raises a hand, and the ghoul **fans out** into twenty
ghouls, each in another palette, sliding apart like a deck of cards, then
snapping back into one. The ghoul looks at its own hands. Django: "..."
Otenko: "Django. Who is that?" The stranger is gone.

Title: **BOKTAI 3, DECOMPILED**, two seconds, straight on.

(How: keyframe sprite packages made from the ghoul's own frames, each naming
a different palette pool, gliding with `997B`; the real ghoul hidden while
the copies are out.)

### The chase: set pieces (0:10–5:30)

Each is a short scene with the stranger somewhere in it, usually walking
off-screen just as Django arrives. Candidates:

* **The doors lie.** Django runs after the stranger through a door, and
  every door after that goes somewhere else: San Miguel, a desert, Dark
  Castle, the Sealed Prison, a dungeon, back to San Miguel, faster and
  faster (real warps between real scenes, the continuation carrying the
  cutscene). He comes out in a closet. Otenko: "I don't think we're
  following it. I think it's moving us."
* **The light show.** A puzzle room. The stranger touches one floor switch
  and every switch, gate and light in the room fires in a cascade, opening
  and closing in time with the music (each switch rewired to our
  procedure; a timed sequence). The way out is open. Django: "I was going
  to solve that."
* **The night falls on cue.** In a town, the stranger snaps its fingers and
  the time of day sweeps from noon to night to dawn in seconds (palettes,
  the solar sensor), shop shutters, townsfolk going in and out of doors in
  sync like clockwork.
* **The boss who forgets.** The Count's fight (Lifeless Town). Mid-fight
  the stranger appears on a ledge; the Count's palette cycles, his HP bar
  drains to one, his attack runs backwards, then he shrinks into a ghoul
  (sprite swap) and walks away. Django lowers his gun.
* **Riding Vanargand.** A silhouette crosses the sky of the Dark Castle on
  Vanargand's back, Vanargand roaring through its rise (receiver commands
  51/52/56/62), the camera chasing. Django: "Is that... riding Vanargand?"
  Otenko: "Nobody rides Vanargand."
* **The prison break.** The stranger presses a big switch and cages drop
  from the ceiling, each holding someone from across the game: Sabata,
  Trinity, the bosses, shopkeepers, Smith. The cages rise again, empty.
  (Each cast member in their real sprite; see "The cast" in the art and
  mechanism checks.) Or a summoning circle of particles, the cast rising
  out of it in a ring. Pick whichever looks better.
* **The cutting room floor.** The stranger walks through a wall into a room
  Konami never shipped (a scene the scripts contain but no door reaches:
  check `areas.py` for scenes with no incoming doors, area 2 "Unidentified
  / unused room", debug scenes). Django follows. It is strange and unlit.
  Otenko: "This place isn't in the world."
* **The ROM as a stage.** The floor becomes the ROM itself: the Studio's
  treemap drawn as an injected tileset and map, colored by kind; the
  stranger walks across it. The most ambitious; prove it early, fall back
  to a static backdrop on a BG layer.

### The overload: the reveal (5:30–6:15)

Django finally corners the stranger in a sunlit room (the Sealed Prison
skylight). He raises the Gun Del Sol. The SUN gauge starts to rise on its
own: full, then past full (the HUD's sun value and the harness's `lux`
pushed beyond anything the game ever shows), still climbing.

Django: "What the... My gun is overheating!"

The gauge cracks. The gun shakes (quake, sparks, particles on a quickening
beat, the music pitched up), then **explodes**: a burst of particles, white
flash, sound cut. The smoke clears and where the stranger stood, its cloak
burned away, is the Claudesona.

Claude: "Sorry. That was me." Then: "I decompiled your game."

### After the reveal: the encore (6:15–7:15)

Short and funny, now with Claude in plain sight:

* **The coffin train.** Django, in Boktai tradition, drags a coffin. Then
  Claude walks past dragging a chain of coffins: one for every boss in the
  game, a train that snakes across the screen and keeps coming, through
  rooms, around corners (coffin packages following a path, each a few
  frames behind the one before). Django: "You're not purifying all of
  those." Claude: "Already did."
* **Riding Vanargand, for real.** Claude jumps onto Vanargand's back (a
  keyframe sprite gliding onto the boss layer's position) and waves.
* **The sun, handed back.** Claude hands Django the sun: the gauge swings
  empty to full, day to night to day, and the Gun Del Sol fires a beam
  across the screen.

### The credits (7:15–8:00)

The game's own credits sequence (scene 6595 "Credits", the save bank's
`0086 Credits` checkpoint), with the decomp's credits inserted between the
real ones in the same style: the decomp contributors, the fan translation
team, vgel and the Claudesona artists, pret's agbcc, decomp-permuter. The
coffin train can pass behind them.

Last shot, Django: "How did you even do all this?" Claude: "Want to see?"
Fade to white; the next shot is the Studio.

## Art pipeline (Codex for references, then real pixel art)

v2's sprites did not fit the game. Do this properly. Two characters:

* **The stranger**: a cloaked, hooded figure, face in shadow, maybe a faint
  warm glow under the hood. It should read as a mysterious Boktai NPC, and
  only in hindsight as Claude (the cloak's outline can hint at the mane).
  Walk, idle, raise-a-hand, snap, vanish frames.
* **The Claudesona**, revealed: walk cycle, idle, talk, cheer, the jump
  (for Vanargand), dragging (for the coffin train), and the frames of the
  cloak burning away for the reveal.


1. **Gather references** under `build/showcase/refs/`:
   * the Claudesona: the CC0 sprites in github.com/N8python/claudesona
     (`chrome-extension/assets/claude_*.png`, view or fetch with the user's
     go-ahead) and stills from the Eidoverse videos (the user posted two:
     a ska band of Claudes in suits, and a Claude in a cowboy hat and yellow
     trousers; search X/YouTube for "eidoverse claude" and SkyeShark's posts);
   * Boktai's own art from the ROM: every in-game cast sprite (the large
     sprites the NPCs and the player use: `bigsprites.py sheet HASH` with the
     right palette pool), the dialogue portraits (`593A`/`593B` with pools
     503–507), and a few enemies and bosses for proportions and shading.
2. **Generate Boktai-style references with Codex**:
   `codex exec -i ref1.png -i ref2.png ... "<prompt>"`. Prompt for: a
   Claudesona character sheet in the exact style of the attached Boktai
   sprites (palette size, outline weight, shading direction, 3/4 view,
   proportions), front/back/side/walk/attack/cheer poses at the sprite
   scale; 32×32 portraits in the style of the attached portraits (several
   expressions: neutral, happy, smug, surprised); the stranger in the same
   style; and the reveal frames (the cloak burning away). Check first
   that this Codex version can produce images (an image generation tool);
   if it cannot, have it produce the pixel art directly as indexed PNGs or
   as a palette plus an index grid, and iterate on its output. Keep the
   prompts and outputs in `build/showcase/refs/` and note what worked.
3. **Make the in-game art** from those references: quantize to a 16-color
   pool, clean by hand (in code, as v2 did) where needed, and check every
   sprite next to Django, Sabata and Otenko in a screenshot at 1x and 4x
   before using it. A Claude walk cycle (4 directions × 3–4 frames), idle,
   talk, cheer, a jump (for Vanargand), dragging, and the reveal
   frames; the stranger's frames. The bar: a fan should not be able to tell it isn't original art.
4. **Portraits** at 32×32 in the style of the game's portraits (bust, the
   same lighting and outline), with expression variants if a speaker can
   carry more than one frame. v2's portrait was a placeholder.

## Part 2: how it was done (the Studio and the tools)

Recorded with `studio_tour.py`, but reworked:

* **No slow scrolls.** Each stop is a still or a short, purposeful
  interaction (open a cutscene, play a song, click a sprite into its
  palettes, hover a treemap block), held long enough to read.
* **A caption panel** beside the page (e.g. the page at 1280×800 on the
  left of a 1920×1080 frame, a text panel on the right): a title and two to
  four lines on what is interesting there. Render the panel as HTML in the
  same headless Chrome (one template page with the Studio in an iframe and
  the caption beside it), so it matches the Studio's look.
* Show every page, the interesting ones longer. Suggested stops and caption
  text (check every number against the repo when recording):

| Stop | Caption |
|---|---|
| Browse home | "One index of the whole game: the story, cast, monsters, places, cutscenes, objects, scripts, sprites, sound and code, all linked." |
| Story | "Story progress is one variable, 0 to 194. We mapped all 173 beats: where each is set, the cutscene around it, the doors it opens." |
| A cutscene page | "Cutscenes are data: 240 tables, 3,651 steps of camera, screen, sound and actor commands, played by one small director. Decoded, then written." |
| Cast (Otenko) | "Every character, their portrait from the game's own speaker table, every line they speak." |
| Bestiary / a monster family | "Monster families: one sprite, many palettes. The ghoul's C option picks pool 402 + C." |
| Actors page | "722 script-callable objects in the registry, named from Konami's own hashed identifiers." |
| Scripts | "The scripts are GCL, the language Konami built for Metal Gear Solid. The engine runs them in a custom virtual machine, which is decompiled. We recovered names from MGS references and debug prints, and render them as readable Python-like code." |
| Places / World | "496 scenes, grouped into areas by their doors." |
| Sprites, Palettes | "Every large sprite and sprite package, with the palettes the game really draws them in, traced through the code." |
| Maps, Tiles | "Stages rebuilt from map, tileset and terrain data, with objects placed where the scripts put them." |
| Sound | "Every MP2K song, played in the browser from the ROM's own sequence data." |
| Text | "The script text, editable, with the fan translation side by side." |
| Saves | "Saves decoded; 83 checkpoints that boot straight into any point of the story." |
| ROM treemap | "The whole 16 MB ROM by what it is and how well we understand it." |
| Code / progress | "31% of the code is matching C: byte-identical when compiled." |
| Tooling (cards, not pages) | the agbcc fork with an instrumented register allocator (`rasolve`: solve allocation in the compiler, then name the C change), the permuter fork, `relatives.py`, the headless harness and shift test (all data relocatable), Centaur (play the game linked to the Studio), the GCL assembler that made this video. |

* **Live edit, split screen.** The Studio on one side and the running game
  on the other (Centaur: mGBA linked to the Studio, `/live`): edit a line
  of dialogue in the Studio's text page and the next message in the game
  says it; change a palette and the sprite recolors. This shows the Studio
  as a tool for changing the game, not only a viewer.
* Close on the decomp's numbers and a thank-you card (the fan translation
  team, vgel and the Claudesona artists, pret's agbcc, decomp-permuter).

## Build and record

* One builder per part: `tools/showcase/v3/` with the scene(s), the art
  (generated sources and the code that turns them into packages and pools),
  and the edit. Keep v1 and v2 building.
* Record the game with sound (the harness's `record`), at adaptive dialogue
  timing, in segments per act so each can be re-recorded alone; cut with
  crossfades and short title cards ("Act II: Something is wrong").
* Output `~/Movies/boktai3-showcase/boktai3-showcase-v3.mp4` (full) and a
  chapter list (timestamps) next to it. Reveal it in Finder when done.

## Order of work

Prove the risky mechanisms first, each in a throwaway scene with a
screenshot, before writing the story around them: ghoul frames as a
gliding package (the hook, first); the SUN gauge pushed past full, a way
to draw it cracking, and the explosion (the reveal); a chain of coffin
packages following a path; a run of chained warps that keeps the cutscene
going; a floor switch firing our procedure and a timed cascade; the time
of day swept in seconds; changing a boss's HP, palette and sprite during
its fight; cast members in their real sprites (in cages or a circle);
riding Vanargand; an unshipped room; the ROM drawn as a stage. Cut any that
fail and say so; the menu has more set pieces than the video needs. Then the art,
then the script, then the Studio part, then the edit.
