# Showcase v3: "The Prologue Breaks" (plan and script)

Tags: #showcase #scripts #tooling

The brief for the third showcase video. v1 and v2 are in
`~/Movies/boktai3-showcase/`; their builders are `tools/showcase/showcase_v1.py`
and `showcase.py` (v2). This one aims as high as it can: a long cut
(8–12 minutes) that we cut down later. Two parts:

1. **The hack (6–8 min).** A cold open that hooks in seconds (the SUN
   gauge overloads, Claude walks out of the light), then a journey through
   the real game where Claude, having decompiled it, rewrites each place as
   he passes through. Everything is played by the game's engine:
   new scripts, cutscene tables, sprites, palettes, stage data.
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

Rules from the user's review:

* **Hook in the first 5–10 seconds.** No slow prologue: the prologue gets
  at most a few seconds as a quick reference.
* **Not one stage.** The video travels through the real game. Fans
  recognize places without setup.
* **Glitch effects for their own sake are dull.** Every effect must be a
  capability of the decompiled game being shown, not noise.
* **No Clawd.** All art effort goes into a Claudesona that looks like it
  belongs in Boktai (sprites and portraits).
* Claude is confident, warm, a little mischievous, proud of the work; the
  cast react in character (Otenko lectures, Django is blunt, Sabata is dry,
  Trinity is sharp).

Dialogue pacing: faster than v2 (which pressed A every 105 frames). Use
adaptive timing, `frames = 30 + 1.8 * characters on the page`, from the
script; the recorder presses A per page on that schedule.

### Cold open: the overload (0:00–0:20)

The Sealed Prison, under the skylight. Django raises the Gun Del Sol. The
SUN gauge fills, and keeps filling past its end (the HUD's sun value and the
harness's `lux`, pushed beyond what the game ever shows). The gauge cracks,
the gun sparks (particles, quake, white flashes on a quickening beat), and
the screen blows out to white. Out of the light walks a figure with a
sunburst head: the Claudesona.

Claude: "Hi. I decompiled your game."

Title card: **BOKTAI 3, DECOMPILED** (two seconds, then straight on).

### Act I: who is this? (0:20–1:30)

Django and Otenko, in the prison room, after the flash. Otenko and Claude
look at each other: two sunflower heads (Otenko: "A spirit of the sun...?
No. You're something else."). Claude explains in three lines: he read every
function, script and table of this world and wrote them back as source code,
so now he can change the world from inside it. Django: "Prove it."

A quick flash of the prologue here, as Claude's proof that he knows this
world's history: the dream stage and Vanargand rising, five seconds, played
from our own copy of cutscene 0 in fast motion, with Claude's voice over it.

### Act II: a tour of a world rewritten (1:30–6:30)

Claude, Django and Otenko travel through the game in story order. Each stop
is a real scene (warps between real scenes; the cast respawned in each; the
continue procedures carry the cutscene from scene to scene, as the prologue
carries scene 6554 through its four entrances; save checkpoints from
`saves.py` give the right story state for each place). Each stop shows one
decoded system, with that place's own music and people:

1. **Sealed Prison: the switches.** A puzzle room. Claude doesn't solve it:
   he walks to the door and it opens because he rewired the room's floor
   switch to his own script; then every switch in the room fires in sequence,
   gates opening and closing like a light show. "Every switch in the game is
   a script. So now are mine."
2. **San Miguel: the people.** The townsfolk are mid-conversation with lines
   Claude has edited (the text bank), and notice: Smith: "Why did I just say
   that?" The shop's stock changes while Django is looking at it (item data).
   Claude buys the Solar Gun back for Django for 0 Sols.
3. **Lifeless Town: the Count.** The boss fight against the Count. Django is
   losing; Claude rewrites the fight live: the Count's palette cycles, his HP
   bar drops to one, his attack pattern runs backwards. The Count, in his own
   portrait: "This... is not how this goes."
4. **Pirate Island: one becomes many.** A single ghoul walks up, then splits:
   copies slide out from it into a fan, each in another palette, then snap
   back into one and it walks off confused. (Keyframe sprites made from the
   ghoul's own frames, converted into sprite packages, each naming a
   different palette pool, gliding with `997B`.) "The game only ever drew
   one ghoul."
5. **The cast, summoned.** Characters from across the game appear in a ring
   around Django, each in their real sprite, each saying one line from the
   game's own script in their own portrait, then gone.
6. **Dark Castle: Vanargand.** Claude conducts Vanargand like an orchestra
   (its receiver commands: rise, arms, roar), then jumps onto his back (a
   keyframe sprite gliding onto the boss layer's position) and rides him as
   the camera pulls back and the music swells.
7. **The cutting room floor.** Claude opens a door to a room Konami never
   shipped: scenes the scripts contain but no door reaches (the area
   labels list candidates, e.g. area 2, "Unidentified / unused room"; check
   `areas.py` for scenes with no incoming doors and debug scenes). "Nobody
   has stood here since 2005."
8. **The ROM as a stage.** The floor becomes the ROM itself: the Studio's
   treemap rendered as a new tileset and map (an injected BG map and
   tileset: the decomp's own understanding map, colored by kind), and
   Claude walks across it pointing out regions ("This is the code. 31% of
   it is ours now. This is every sprite."). The most ambitious beat; prove
   it early, have a fallback (a static backdrop image on a BG layer).
9. **The sun, finished.** Back to the opening's idea: Claude hands Django
   the sun, the SUN gauge swings from empty to full, day to night to day
   (time of day, the solar sensor value), the Gun Del Sol fires a beam
   across the screen.

Reactions between stops, short: Django ("This is cheating." / "Can you give
me infinite energy?"), Otenko ("The order of this world, rewritten this
easily..."), Trinity in San Miguel ("Who's the flower?").

### Act III: the credits (6:30–7:30)

The game's own credits sequence (scene 6595 "Credits", from the save bank's
`0086 Credits` checkpoint), with the decomp's credits inserted between the
real ones in the same style: the decomp contributors, the fan translation
team, vgel and the Claudesona artists, pret's agbcc, decomp-permuter.

Then, in the last shot, Django: "How did you even do all this?" Claude:
"Want to see?" Fade to white; the next shot is the Studio.

## Art pipeline (Codex for references, then real pixel art)

v2's sprites did not fit the game. Do this properly:

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
   expressions: neutral, happy, smug, surprised); and the emerging-from-
   the-light frames for the cold open. Check first
   that this Codex version can produce images (an image generation tool);
   if it cannot, have it produce the pixel art directly as indexed PNGs or
   as a palette plus an index grid, and iterate on its output. Keep the
   prompts and outputs in `build/showcase/refs/` and note what worked.
3. **Make the in-game art** from those references: quantize to a 16-color
   pool, clean by hand (in code, as v2 did) where needed, and check every
   sprite next to Django, Sabata and Otenko in a screenshot at 1x and 4x
   before using it. A Claude walk cycle (4 directions × 3–4 frames), idle,
   talk, cheer, a jump (for Vanargand), a pointing pose, and the emerging
   frames. The bar: a fan should not be able to tell it isn't original art.
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
screenshot, before writing the story around them: the SUN gauge pushed
past full (and a way to draw it cracking); ghoul frames as a gliding
package; a character summoned with a real sprite; a cross-scene transition
that keeps the cutscene going; a floor switch firing our procedure;
changing a boss's HP and palette during its fight; riding Vanargand; an
unshipped room; the ROM drawn as a stage. Cut any that fail and say so. Then the art,
then the script, then the Studio part, then the edit.
