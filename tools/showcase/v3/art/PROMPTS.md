# v3 art: provenance and the Codex prompts

The frames in this directory were drawn with Codex's image generation (`codex exec -i REFS... PROMPT`,
codex-cli 0.160.0, image_generation enabled) and turned into true-resolution indexed sprites
with `../pixelate.py` (pitch detection, center sampling, one shared palette). References: the
CC0 Claudesona sprites (github.com/N8python/claudesona, `chrome-extension/assets/claude_*.png`)
and, for style only, the game's own sprites and portraits rendered from the ROM (kept under
`build/showcase/refs/`, never committed). Nothing here is ROM data.

* `stranger/`: stranger_sheet_v1 (first prompt below), used as generated.
* `reveal/`: reveal_r1a (the cloak burning away); its last frames set the Claudesona's look.
* `claude/`: claudesona_sheet_r3a, asked to match the reveal's last frames.
* `portraits/`: claude_portraits_v1, 32x32 cells sampled from the 8x sheet.

What worked: give Codex the game's sprites at 4x next to the character refs and ask for the
sheet at an exact 4x on flat green; then a second round with a side-by-side comparison
(`../compare.py`) of its first try against the game's NPCs and the specific differences
(tones per material, face pixels, proportions). Asking a later sheet to match frames it had
already drawn kept the character consistent across sheets.

## prompt_stranger_sheet

```
Use your image generation tool. Save every generated image as a PNG under ./out/ with the file name given below, and report the paths.

Attached references: ref_boktai_cast.png and ref_boktai_django.png are real in-game sprites from the Game Boy Advance game Boktai 3 (Shin Bokura no Taiyou), shown at 4x. ref_claudesona_*.png show a fan character (a round white face inside a mane of orange petal rays) who will later be revealed to be under the cloak.

Task: draw "the stranger", a mysterious cloaked NPC, as if Konami's own artists had made it as a sprite for Boktai 3, in exactly the style of the attached game sprites:
- a figure in a long, hooded, dusty cloak (deep plum or dark charcoal-violet, with a worn hem and a little clasp), the face hidden in shadow inside the hood with only a faint warm orange glow where the face would be; the hood is slightly wide and lumpy at the top, as if something spiky (a mane) is folded under it, but nothing orange shows except the glow; the hands are hidden in the sleeves;
- about 24 px wide and 40 px tall at 1x (the same size as the NPCs in ref_boktai_cast.png), the same 3/4 top-down camera angle and chibi proportions;
- a 1-px dark outline, flat cel shading with 2 to 3 tones per material lit from the top-left; at most 15 colors; hard pixels, no anti-aliasing, no gradients, no blur;
- shown at exactly 4x nearest-neighbor scale (each sprite pixel a crisp 4x4 block) on a flat solid pure green #00FF00 background, in a grid with at least 16 px between sprites.

Poses: row 1: standing facing down-right, down-left, up-right (back), up-left; row 2: walk cycle facing down-right, 4 frames; row 3: raising one hand (sleeve falling back to show an orange glove), snapping fingers, pointing, turning away; row 4: vanishing into motes of light, 4 frames (the cloak dissolving from the bottom up into small square sparkles).

Save as out/stranger_sheet_v1.png. Then make a second variation saved as out/stranger_sheet_v2.png.
```

## prompt_reveal

```
Use your image generation tool. Save every generated image as a PNG under ./out/ with the file name given below, and report the paths.

Attached: ref_cmp_stranger_r1.png shows real Boktai 3 sprites (left) and our hooded stranger sprite (right) at 1x and 4x; ref_claudesona_*.png show who is under the cloak (the Claudesona: a round white face with simple black features inside a mane of about 12 orange petal rays, a lavender top, light-blue jeans, brown shoes, an orange lion tail); ref_boktai_cast.png shows more real game sprites at 4x.

Draw the reveal: the stranger's cloak burns away and the Claudesona is standing there, as a sequence of 8 sprite frames in exactly the style of the attached game sprites (about 24 px wide and 42 px tall at 1x, the same 3/4 top-down angle facing down-right, black 1-px outline, 3-4 tone cel shading lit from the top-left, at most 15 colors, hard pixels, no anti-aliasing):
1. the stranger as in the reference, a few embers at the hem
2. glowing orange cracks run up the cloak
3. the cloak's lower half burns away into bright yellow-orange embers, jeans and shoes showing
4. the cloak is gone to the chest, embers rising, the lavender top showing
5. only the hood left, smoking, petals poking out of it
6. the hood bursts: the orange petal mane springs out, a few last embers, the face still squinting
7. the Claudesona standing, slightly sooty, eyes open, small sheepish smile, a wisp of smoke from the mane
8. the Claudesona waving with one hand, friendly

Show the frames left to right at exactly 4x nearest-neighbor scale (every pixel a crisp 4x4 block) on flat pure green #00FF00, at least 24 screen px apart. Save as out/reveal_r1a.png, then a variation as out/reveal_r1b.png.
```

## prompt_claude_sheet

```
Use your image generation tool. Save every generated image as a PNG under ./out/ (create it if needed) with the file name given below, and report the paths.

Attached references:
- ref_claudesona_*.png: the Claudesona, a fan character (CC0 art). A round white face with simple black line features (closed happy eyes, small smile), a mane of about 12 long rust-orange petal rays around the head like a sunflower or a starburst, a lavender long-sleeved top, light-blue jeans, dark brown shoes, orange hands, and a curled orange lion tail.
- ref_boktai_cast.png and ref_boktai_django.png: real in-game sprites from the Game Boy Advance game Boktai 3 (Shin Bokura no Taiyou), shown at 4x. Django is the boy with the red scarf.

Task: draw the Claudesona as if Konami's own artists had made it as an NPC sprite for Boktai 3, in exactly the style of the attached game sprites:
- about 24 px wide and 40 px tall at 1x (the same size as the NPCs in ref_boktai_cast.png), chibi proportions with a big head, the same 3/4 top-down camera angle;
- a 1-px dark outline (near-black brown, as in the refs), flat cel shading with 2 to 3 tones per material, lit from the top-left; at most 15 colors in the whole sheet; hard pixels, no anti-aliasing, no gradients, no blur;
- the petal mane must read clearly at this size (chunky orange rays with a darker orange inner shade), the face a small white disc with two dot or arc eyes;
- show it at exactly 4x nearest-neighbor scale (every sprite pixel a crisp 4x4 block) on a flat solid pure green #00FF00 background, poses laid out in a grid with at least 16 screen px of empty space between sprites.

Poses (one sprite each), rows:
1. standing idle facing down-right (front 3/4), facing down-left, facing up-right (back 3/4), facing up-left
2. walk cycle facing down-right, 4 frames
3. walk cycle facing up-right, 4 frames
4. talking (mouth open), cheering (both arms up), pointing forward, jumping (knees up, arms out), dragging a rope over its shoulder while leaning forward (as if pulling a heavy coffin)

Save as out/claudesona_sheet_v1.png. Then generate a second variation with the same instructions, saved as out/claudesona_sheet_v2.png.
```

## prompt_claude_sheet_r2

```
Use your image generation tool. Save every generated image as a PNG under ./out/ with the file name given below, and report the paths.

ref_cmp_claude_r1.png shows, left to right, three real Boktai 3 sprites (Django, an old man, a girl) and then our first attempt at the Claudesona as a Boktai 3 NPC sprite, at 1x (top strip) and 4x (bottom). The attempt is close but does not yet look like Konami's art. Compared with the real sprites it is too flat and too simple:
- the real sprites use 3 to 4 tones per material (a highlight, a base, one or two shades) with clear form shading lit from the top-left, and darker, warmer interior shades next to the black outline;
- their faces have features drawn with a few carefully placed pixels; ours is a featureless cream square;
- the mane reads as a spiky orange blob: it should read as about 12 distinct petal rays, each petal with a light top edge and a dark lower edge, arranged as a ring around a round white face, with a darker orange inner ring where the petals meet the face;
- proportions: the real NPCs are about 24 px wide and 40-45 px tall with the head about 16-18 px; the body should have some mass (shoulders, a torso) like the old man and the girl.

Also attached: ref_claudesona_*.png (the character's design: white round face with simple black features, orange petal mane, lavender long-sleeved top, light-blue jeans, dark brown shoes, orange hands, curled orange lion tail) and ref_boktai_cast.png / ref_boktai_django.png (real game sprites at 4x).

Draw a new sprite sheet of the Claudesona that fixes all of this, in exactly the pixel-art style of the real Boktai 3 sprites: about 24 px wide and 42 px tall at 1x, the same 3/4 top-down angle, black 1-px outline, 3-4 tone cel shading lit from the top-left, at most 15 colors in total (transparent background not counted), hard pixels, no anti-aliasing, no blur. Show it at exactly 4x nearest-neighbor scale (every pixel a crisp 4x4 block) on flat pure green #00FF00, with at least 24 screen px between sprites.

Poses, rows:
1. standing idle facing down-right (front 3/4), facing down-left, facing up-right (back 3/4: the back of the mane, no face), facing up-left
2. walk cycle facing down-right, 4 frames
3. walk cycle facing up-right, 4 frames
4. talking (mouth open), cheering (both arms up, eyes closed happy arcs), pointing forward, jumping (knees tucked), waving
5. dragging: leaning forward and pulling a rope over the right shoulder, 2 walk frames; sitting astride (legs apart, as if riding a big animal), arms raised in triumph

Save as out/claudesona_sheet_r2a.png, then make a second variation as out/claudesona_sheet_r2b.png.
```

## prompt_claude_sheet_r3

```
Use your image generation tool. Save every generated image as a PNG under ./out/ with the file name given below, and report the paths.

ref_reveal_r1a.png is a strip of 8 sprite frames we made: a hooded stranger whose cloak burns away to reveal the Claudesona (frames 7 and 8 show her fully). Frames 6-8 are the best version of the Claudesona so far: keep EXACTLY that design, size, palette and pixel style (round white face with simple black dot eyes and a small smile, a ring of chunky orange-red petal rays with a darker inner edge, lavender top, light-blue jeans, brown shoes, orange hands, curled orange tail, black 1-px outline), at the same scale as those frames. ref_boktai_cast.png and ref_boktai_django.png are real Boktai 3 sprites at 4x for the overall style.

Draw a full sprite sheet of that same character, matching frames 7-8 of ref_reveal_r1a.png pixel for pixel in style (same palette, same proportions: about 24 px wide and 42 px tall at 1x; the same 3/4 top-down view), shown at exactly 4x nearest-neighbor scale (every pixel a crisp 4x4 block) on flat pure green #00FF00, at least 24 screen px between sprites, poses in rows:
1. standing facing down-right, facing down-left, facing up-right (back of the mane), facing up-left
2. walking facing down-right, 4 frames (legs and arms swing)
3. walking facing up-right, 4 frames
4. talking (mouth open), cheering (both arms up, happy closed eyes), pointing forward, waving, shrugging sheepishly (one hand behind the head)
5. dragging: leaning forward pulling a chain over the shoulder, 2 walking frames; riding: sitting astride with legs apart and one arm raised, as if riding a big beast; jumping (knees tucked, arms out)

Save as out/claudesona_sheet_r3a.png, then make a second variation as out/claudesona_sheet_r3b.png.
```

## prompt_portraits

```
Use your image generation tool. Save every generated image as a PNG under ./out/ with the file name given below, and report the paths.

Attached references: ref_boktai_portraits_a.png and ref_boktai_portraits_b.png are the real 32x32-pixel dialogue portraits from the Game Boy Advance game Boktai 3, shown at 3x with a gray gutter. ref_claudesona_*.png show the Claudesona, a fan character: a round white face with simple black line features inside a mane of about 12 long rust-orange petal rays, lavender top.

Task: draw dialogue portraits of the Claudesona as if Konami's own artists made them for Boktai 3, in exactly the style of the attached game portraits: a 32x32-pixel bust, the face filling most of the square and cropped by the edges as in the refs, a 1-px black frame line around the square, bold black outlines, flat cel shading with 2-3 tones per material, bright saturated colors, at most 15 colors per portrait, hard pixels, no anti-aliasing. The Claudesona's face is a white disc with simple black features; the orange petal rays fill the corners; a hint of the lavender collar at the bottom.

Make one image with four portraits side by side, each shown at exactly 8x nearest-neighbor scale (each pixel a crisp 8x8 block, so each portrait is 256x256), separated by 32 px of flat pure green #00FF00: 1. neutral, friendly; 2. happy (closed curved eyes, open smile); 3. smug and mischievous (one eye winking, a sly half smile); 4. surprised (round eyes, small o mouth). Save as out/claude_portraits_v1.png. Then make a second variation saved as out/claude_portraits_v2.png.
```
