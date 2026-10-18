# Showcase v3: "The Prologue Breaks" (plan and script)

Tags: #showcase #scripts #tooling

The brief for the third showcase video. v1 and v2 are in
`~/Movies/boktai3-showcase/`; their builders are `tools/showcase/showcase_v1.py`
and `showcase.py` (v2). This one aims as high as it can: a long cut
(8–12 minutes) that we cut down later. Two parts:

1. **The hack (5–7 min).** The game's own prologue starts normally, then
   the world starts changing under the characters' feet. Claude arrives,
   explains that it decompiled the game and can now rewrite it from inside,
   and shows what that means. Everything is played by the game's engine:
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

Tone: the original prologue played straight, then a fourth-wall break. The
game knows something is wrong before Claude explains. Claude is confident,
warm, a little mischievous, and proud of the work; the characters react the
way Boktai characters would (Otenko lectures, Django is blunt, Sabata is
dry).

Dialogue pacing: about twice v1's speed. v2 pressed A every 105 frames
(1.75 s); go to roughly 90 frames for a two-line page, and make it adaptive:
`frames = 36 + 2 * characters on the page`, computed from the script, so
short lines don't linger. The recorder presses A per page on that schedule.

### Act I: the prologue, as shipped (0:00–1:00)

Play the real prologue opening with real assets, slightly trimmed so it
reads at video speed: the text crawl (scene 6551) over its music, then
Sabata waking in the dream (scene 6554, cutscene 0: "Where... am I?", the
camera pan, Vanargand rising). This sets the baseline: this is the game,
untouched. (It can be a straight recording of `tests/prologue.txt` up to
cutscene 0's end, or our own copy of cutscene 0 with tighter waits.)

### Act II: glitches (1:00–2:00)

The prologue keeps going, then frays:

* A line of Sabata's dialogue repeats itself, then the text window shows a
  line from somewhere else in the game (pick a famous one), then garbage
  glyphs, then corrects itself. (Strings on borrowed ids; a message that
  pages through those strings.)
* The floor's palette flickers to another stage's palette and back
  (`set_stage_palette` with another area's palette, then the dream's).
* Vanargand's rise plays backwards (its commands in reverse order), or
  freezes mid-rise while the screen quakes with no sound.
* A ghoul walks across the dream where none belongs, in the wrong palette,
  then blinks out.
* Otenko appears (he is not in this scene in the real game): "Sabata?! No,
  this isn't right. This dream isn't supposed to have... me in it."
* Sabata: "Something is rewriting this place."
* The camera snaps to empty space, the screen tints, the music stops dead
  (`fade_out_bgm` speed 0 or a silent song).

### Act III: the arrival (2:00–3:00)

Otenko begins to glow and **transforms into Claude**. Otenko and the
Claudesona share a silhouette (a round face in a sunburst), so the
transformation sells itself:

1. Otenko's sprite flashes white (screen fade to white at partial strength,
   repeated at a quickening tempo), particles orbit him (`particles_at`).
2. His palette shifts step by step toward Claude's terracotta (write a
   series of palette pools and switch between them, or a run of keyframe
   frames that morph Otenko's petals into Claude's: drawn frames, see the
   art pipeline).
3. A burst (quake, flash, sound), and Claude stands where Otenko was; Otenko
   is beside him, dazed (or reappears, now separate).

Claude: "Sorry about the entrance. I'm Claude. I've spent the last while
reading this game, every function, script and table, and writing it back as
source code. Now I can change it from inside."

Sabata: "...You're the one rewriting my dream."

Claude: "Borrowing it. It's the best stage for a demonstration."

Clawd arrives too (Claude Code's mascot), as comic relief and Claude's
helper.

### Act IV: demonstrations (3:00–6:00)

Each demo is a beat with a short lead-in line and a reaction:

1. **One becomes many.** A single ghoul splits apart: copies slide out
   from the original into a fan, each in a different palette (the effect v1
   had and v2 lost; make it better). Do it with keyframe sprites made from
   the ghoul's own frames, converted into a sprite package (large sprite
   frames → package; `bigsprites.py` decodes them), so each copy can glide
   (`997B`) and each package names a different palette pool. Then the copies
   snap back into one. "Same sprite, one palette option each. The game only
   ever drew one ghoul."
2. **The cast, summoned.** Characters from across the game appear in a ring
   around Sabata, each with their real sprite: Django, Otenko, Trinity,
   Lita, Zazie, Smith, the Count, Carmilla... (`place_character` ids,
   `SceneNpc` spawns or keyframe packages built from their sprites). Each
   speaks one line in their own portrait, lines taken from the game's own
   script ("Yo, Django!"), then they vanish one by one.
3. **Places, swapped.** The stage itself changes: the dream fades and Sabata
   and Claude are standing in San Miguel, then the Sealed Prison, then the
   world map, each held for a beat (door transitions to real scenes with our
   cast respawned in each; the entrance and continue procedures carry the
   cutscene from scene to scene, as the prologue does with scene 6554's four
   entrances).
4. **Triggers.** A floor switch appears at Claude's feet; Claude steps on
   it and a gate rises, torches light, a chest drops (the game's own
   floor-switch, trap and chest objects with `trap` events wired to our
   procedures). "Every switch in the game is a script. So now are mine."
5. **A fight.** Claude walks into a room of enemies (or the enemies are
   summoned) and fights: walking (`walk_to`, and the player commands for
   attacks and poses), enemies attacking, hit flashes, enemies defeated in
   palette-swapped bursts. If driving combat from a table proves too hard,
   the player character can be Claude: a Claude sprite over the player (or
   the player's palette and portrait swapped) while the recorder plays the
   fight with real button input in the harness.
6. **Vanargand, directed.** Claude conducts Vanargand like an orchestra:
   rise, arms, roar, back down, on command (its receiver commands), with the
   music cued to it.
7. **The sound test.** Claude snaps through songs, each named on screen in
   the dialogue box ("Music 28: the Sealed Prison"), ending on the main
   theme.
8. **The sun.** Boktai is a solar game: Claude turns the sun up (the solar
   sensor value the harness feeds, `lux`), the SUN gauge fills, the screen
   brightens; then night falls on command (time of day if scriptable).

Reactions between demos: Django ("This is cheating." / "Can you give me
infinite energy?"), Otenko ("The order of this world... rewritten so
easily..."), Sabata ("Hmph. Show-off.").

### Act V: why, and how (6:00–7:00)

Otenko: "Why are you doing this?"

Claude: "Because nobody had the source. Now anyone can read how this world
works, fix it, translate it, mod it. And it was fun."

Django: "How did you even do it?"

Claude: "Want to see?" The screen fades to white; the next shot is the
Studio.

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
   scale; a 32×32 portrait in the style of the attached portraits; the
   same for Clawd; and Otenko-to-Claude transformation frames. Check first
   that this Codex version can produce images (an image generation tool);
   if it cannot, have it produce the pixel art directly as indexed PNGs or
   as a palette plus an index grid, and iterate on its output. Keep the
   prompts and outputs in `build/showcase/refs/` and note what worked.
3. **Make the in-game art** from those references: quantize to a 16-color
   pool, clean by hand (in code, as v2 did) where needed, and check every
   sprite next to Django, Sabata and Otenko in a screenshot at 1x and 4x
   before using it. A Claude walk cycle (4 directions × 3–4 frames), idle,
   talk, cheer, attack, and transformation frames; Clawd idle, walk, wave.
4. **Portraits** at 32×32 in the style of the game's portraits (bust, the
   same lighting and outline), for Claude and Clawd, and expression variants
   if a speaker can carry more than one frame.

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
screenshot, before writing the story around them: ghoul frames as a gliding
package; a character summoned with a real sprite; a cross-scene transition
that keeps the cutscene going; a floor switch firing our procedure; driving
the player's attack and an enemy's defeat; the transformation. Then the art,
then the script, then the Studio part, then the edit.
