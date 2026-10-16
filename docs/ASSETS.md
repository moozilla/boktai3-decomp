# Resources and asset formats

How the game finds its graphics, maps, fonts and scripts, and the formats
decoded so far. Tools are in `tools/assets/`; everything they write is
ROM-derived and stays under the ignored `build/assets/` tree (see
[DATA.md](DATA.md) for the commit policy).

## Resource lookup

Resources are named by 16-bit string hashes (`sub_08219BD8`, see DATA.md).
`ScriptProc_Start(type, name)` (`0821A520`) looks one up:

1. `type` is the hash of a file extension. The function maps it to a 32-bit
   directory key and binary-searches the directory at `08614D6C` (`0821A4E8`):
   eight-byte records `(u32 key, archive pointer)`, zero-terminated.
2. `name` is searched in that archive (`0821A490` → `0821A454`).

Types whose extension is known (the type is the hash of the string):

| Type | Extension | Archive | Entries | Contents |
|---|---|---|---|---|
| `922E` | `anm` | `08614DCC` | 75 | animation scripts |
| `92B3` | palette | `088223C4` | 195 | palettes: 20-byte header + 16 × 16 colours (532 B each) |
| `9B1B` | ? | `08D530DC` | 1 | sprite palette pool: `u16 count` (955), `u16 ?` (183), then 16-colour sub-palettes (below) |
| `98F5` | ? | `08A2D944` | 1 | 1.7 MB: 381 large sprites (enemies, props, boss parts; below) |
| `A635` | `fnt` | `089F8ED8` | 1 | font |
| `A8D9` | `gcx` | `08D5A85C` | — | compiled scripts, a single image (MGS's GCL file extension); not an archive (below) |
| `AE6C` | ? | `087C4C98` | 528 | LZ77; magic `HP`: stage terrain (height grid, named boxes, patrol paths, nav mesh; below) |
| `C091` | `map` | `08617084` | 287 | LZ77; decompressed header magic `MP` |
| `CB05` | ? | `08BDDE08` | 46 | sprite packages (below) |
| `CEEF` | `ptn` | `0883BDA8` | 1 | tileset library (below) |
| `CEAA` | ? | `08A2AF80` | 1 | 10 KB blob |

`tools/assets/archive.py list | entries TYPE | extract [TYPE...]` lists and
extracts them.

### Archive layout

Offsets are relative to the archive start: `+0 u32 count`, `+4 u32 hash table`,
`+8 u32 offset table`, `+C u32 data start`. The hash table holds `count`
sorted u16 name hashes; the offset table `count` u32 offsets. An entry ends
where the next begins.

## Sprite packages (type `CB05`)

`tools/assets/sprites.py index | info HASH | sheet HASH [--palfile OBJPAL.bin]`.
`ResourceHeader_Relocate` (`082196C4`) copies the 32-byte header and rebases
its five offsets.

| Offset | Content |
|---|---|
| `+0x00` | `u16` palette: the `9B1B` pool entry of palette bank 0, then counts: frames, animations, parts, keys, tiles |
| `+0x0C` | frames, 20 B: `u16 id, u16 parts, u32 ?, s16 x, s16 y, u16 w, u16 h, u32 part offset` |
| `+0x10` | animations, 8 B: `u16 id, u16 keys, u32 key offset` |
| `+0x14` | parts, 8 B: `u16 attr, s16 x, s16 y, u16 tile 0-11 / palette bank 12-15` |
| `+0x18` | keys, 4 B: `u16 frame index, u16 duration` |
| `+0x1C` | 4bpp tiles, stored as a sheet 16 tiles wide |

A part's `attr` is `size << 10 | shape << 8 | vflip << 3 | hflip << 2` (GBA OBJ
shape and size); its last word is a 12-bit tile number and a palette bank in
bits 12-15. Eight packages have more than 1024 tiles (`DE23` has 4096) and use
bits 10-11 of the tile number; reading it as OAM attribute 2 (10-bit tile)
drew the wrong tiles, for example two bodies for Django. An OBJ's tile rows
are 16 tiles apart in the sheet.

**Sprite palettes.** A package's first word is its palette: part bank `b` uses
entry `word0 + b` of the `9B1B` pool (Django `DE23`: 42; the title screen
`731D`: 785). The palette manager (`sub_08217430` and neighbours) copies pool
entries into OBJ banks as sprites need them: IWRAM `0x030051F0` holds the
pool entry per OBJ bank (`0x030051CC` a count), `0x03004DC0` the shadow of OBJ
palette RAM (`0x03004FC0` of BG palette RAM), faded or tinted before it is
copied to the hardware. Across 359 captures, 2,297 of 5,493 used banks equal
their pool entry exactly; the rest are fades (survey warps caught mid-fade)
and a few banks flagged with the high bit (`0xB100`). The package word is a
default: 618 sprite/bank pairs follow it, while menus draw some UI packages
with other entries (`1289` buttons with entry 939, not 807). Large sprites
(`98F5`) carry no palette; captures record which entry their bank held
(`build/centaur/sprite_palettes.json`, `pool`).

The bank table is rebuilt every frame as sprites are drawn: `sub_08217298`
clears it, `sub_082172E0` finds or adds a pool entry (in r4) and returns its
bank, reached from the draw path `08214A45` ← `08219E27` ← `08215501` ←
actor code; an IWRAM routine around `0x03000078` does the same for other
callers (r0 = pool entry). So a large sprite's entry is passed in by the
actor that draws it. The actor sets it once: `AnimData_Load(anim, hash)`
(`082151E4`) picks the sprite and `AnimData_SelectPaletteBank(anim, entry)`
(`08215284`) stores the pool entry in the anim (`+6`, and `+0xC` the entry's
address, `*0x030042E4 + 32 * entry`; entries at or past `*0x030042E0` are
ignored). `tools/assets/bigpalettes.py` pairs the two calls where both
arguments are constants: 35 sprites, used by the Studio when no capture saw
the sprite. One sprite can take several entries, and then each is a
different monster or state: `7E82` is the statue (549) and the armor (551),
and ten routines draw `7E7D` in ten entries (286, 439-453). Sprites whose
hash comes from a table or a script option (`FloorSwitch` `T=`) are not
found this way; their entries are also called with `0x16C`/`0x16D` by the
floor switch (pressed and released), so palette swaps mark states too.

**Items, swords and accessories** (`tools/assets/items.py` → `data/items.csv`).
Their names are text strings at a fixed base plus the ID: items 988 + ID
(descriptions 1043 + ID; valuables such as the Dark Card are items 0x27 and
up), swords 817 + ID, accessories 1107 + ID. The bank keeps the other menu
lists the same way: gun frames from 942, lenses from 970, dungeon floors from
1204, places from 1266. The bags are in GameState: items `+0xA0` (u16 per
slot, `0xFFFF` empty), accessories `+0x160` (u16), swords `+0x1C0` (16-byte
records, ID first), valuables `+0x838` (u16). Menu captures show each slot's
icon: sprite package `5D04` (menu icons, pool entry 884) frame 240 + ID for
swords, 289 + ID for accessories, 337 + ID for items 0x00-0x1D, 370 for every
tarot card (0x20-0x26), 333 + ID from 0x27. No table holding these frames or
string IDs turned up in ROM; the menu code presumably adds the bases.

**Stage palettes** are named by the scripts: a stage load `proc_884(map,
tileset, hp)` is followed by `proc_888(palette, 0, 0)` (194 stages); 104 of the
166 palettes learned from captures equal that argument.

### What the packages show (tentative, from renders)

Rendered with `sprites.py index`. Identifications are visual guesses unless a
code reference is given.

* Player and story characters: `0658` (sword-wielder), `158A` (man in coat),
  `32BA`, `DE23` (915 frames; a playable character), `761E` (1253 frames),
  `FB81`, and a row of townspeople `DFCE`–`E04E`, `E2DE`–`E349`.
* Enemies and bosses: `3DB6` (a small black winged creature with a lash
  attack, loaded by `sub_080B43BC`), `AE52` (a skeleton, 322 frames, loaded by
  `sub_080C3260`), `3D99`, `B3DE`, `F761` (a large beast), `F771` (a large
  bird), `F7D2` (a boss face), `A2DB` (an eruption effect).
* Effects and UI: `1BC7` (flames), `1289`/`E2AB` (frames and boxes), `546B`,
  `5D04`, `DF11` (light bulb), `B991`, `F099`.
* Text graphics: `731D` (the copyright line), `7C8B`, `82F1` ("PURIFICATION
  COMPLETED!"), `E89F` (the title of Boktai 2, 続・ボクらの太陽).

Many `ScriptProc_Start(0xCB05, name)` calls in the actor code (for example
`9247` in `sub_0808D1D8`, `D635`-`D638`, `5290`/`5291`) name packages that are
in no archive. They may be resources of cut or Boktai 2 content whose code
remained; script references (`gcx`) should settle which actors are used.

## Maps (`map`, type `C091`), tilesets (`ptn`) and stages

Found by following the field scene of the late-game save: BG2 is fed every
frame by `BgGfx_LoadTilemap` (`08216584`) from a map decompressed into EWRAM,
and its tiles were copied once by `sub_08215D40` from `ptn`.

**`MP` maps.** 252 of the 287 `map` entries are LZ77-compressed (a 4-byte
prefix, then the map); 35 are stored raw. A map starts `"MP\0\0"`, then
`u16 layers, u16 tiles, u16 metatiles`, `u32 layer table, u32 tile data (0 =
tiles come from a ptn tileset), u32 metatile table` (offsets from `MP`).
A metatile is four BG tile entries (top-left, top-right, bottom-left,
bottom-right; tile, h/v flip and palette bank as in GBA text BG maps). A
layer-table record is `u16 width, u16 height` (in metatiles), `u32 name hash,
u32 offset` of a grid of `u16` metatile indices, stored in blocks of 16 × 16
metatiles when both sides are multiples of 16, blocks row-major. Menu and
screen backgrounds are raw maps with their own tiles and several named layers.

**`ptn`.** One entry: `u16 tilesets` (263), then `u32 table, u32 index lists,
u32 tile pool` at `+0x0C`. A table record is `u16 id, u16 tile count, u32
offset` into the index lists; each index selects a 4bpp tile in the pool of
49,638 tiles. A stage's tiles are its tileset's tiles in order, copied to the
start of the BG character block.

**Stages.** The scripts load a stage with `proc_884(map, tileset, hp)`, which
calls into the `map` command and creates the `Background_CreateFromScript`
actor; 398 distinct triples appear. The third argument names an `AE6C` entry
(LZ77, magic `HP`), the stage's terrain (below). The
field stage of the save is `proc_884 0xcf70 0xf8ea 0x7239` (procedures 1491,
3241, 6388): map `CF70` (32 × 48 metatiles), tileset `F8EA` (351 tiles), BG
palette `92B3:E39B` banks 0-6 (matched against palette RAM; the script does
not name it, so the palette is chosen elsewhere, perhaps by area or time of
day). The OBJ palette banks 0-5 of that scene match no palette resource, so
sprite colours are assembled at runtime.

The studio's Maps view renders every stage and map this way
(`tools/studio/serve.py`, `/gfx#maps`), with the stage data below drawn over it.

### Stage terrain (`HP`, type `AE6C`)

`tools/assets/hp.py list | show HASH | json [HASH...]` decodes all 528
entries. `Terrain_LoadHP(hp)` (`08225C48`) finds the resource and hands the four offsets of
its header (`"HP\0\0"`, then `u32` offsets from `HP`) to the terrain map
`gUnk_030052F4` (`struct EngineMap`, include/engine.h):

| Offset | Section | Loader | `EngineMap` field |
|---|---|---|---|
| `+0x04` | height grid | `Terrain_SetGrid` | `+0x04` (`cells`) |
| `+0x08` | named boxes | `Terrain_SetBoxes` | `+0x0C` |
| `+0x0C` | patrol paths | `Terrain_SetPaths` | `+0x10` |
| `+0x10` | nav meshes (0 = none) | `Terrain_SetNavMeshes` | `+0x14` |

* **Grid.** `u32 0, u16 width, u16 height` (cells; `Terrain_UpdateSize` copies them
  to `gUnk_030052F8`/`FC`), `s16 origin x, s16 origin y` (BG map pixels × 16),
  then `width × height` four-byte `struct SceneCell`s from `+12` (as
  `scene_object.c` indexes them): `u8 height` (low nibble = level, high nibble
  = kind: 0 flat, 1 slope), `u8 tile`, `u16 flags`. Seen in the first dungeon:
  `0x0003` outside the stage, `0x040B` walls, `0x0000` floor, `0x000A` on the
  cell of a door (bit 3 = blocks entry, from the scene-object code).
* **Boxes.** `u32 count`, then 12 bytes each: `u16 x0, z0, x1, z1` (world
  units), `u8 y0, y1` (world units / 16), `u16 name hash`. Scripts refer to
  them by name: doors (`CD22`, `CD23`, ... in nearly every stage), the camera
  area `A679` (`proc_895 0xa679`), traps (`trap 0x4ca7 ...`).
  `cmd 0xB745 0xDA5A name` copies a box into the script work area
  (`r700[0x72]`...), which `proc_931` uses to place the player at a door.
* **Paths.** `u32 count`, `count × (u16 points, u16 offset)` from the section,
  then eight-byte points `u16 x, z, u8 wait, u8 direction (256 = turn), u16
  flags`. Enemy spawners take a path index (`proc_1496 name path ...`).
* **Nav meshes.** `u32 count`, `count` offsets; a mesh is `u16 rects, u16
  portals, u32 extra`, then offsets (from the mesh) of 16-byte rects (`u8 x0,
  z0, x1, z1` in cells and 12 portal indices, `0xFF` = none), 12-byte portals
  (`u16 x, z, ?`, two rect indices) and `extra` u16 values (unknown). Probably
  for enemy pathfinding; only the layout is known.

**World coordinates and the map projection.** Positions (the player's at
`GameState+0x30/+0x32/+0x34`, s16 x, y, z; script arguments) use 256 units per
cell and per height level: x and z run along the grid, y is the height (a
floor of level 1 is y = 256). The BG map pixel of a point is

    px = origin_x / 16 + 3 * (x - z) / 32
    py = origin_y / 16 + 3 * (x + z - y) / 32

so a cell is a 48 × 48 diamond. Fitted to a replay of `first_level_from_start`
(stage 7EFB, scene 2698, and ED78, scene 2705): every still frame matched to
under a pixel, and climbing the stairs (y 256 → 619) confirmed the y term.
The camera keeps the player at screen (120, 94).

**BG scroll in RAM.** The scroll registers are write-only, but the game keeps
each background's full scroll in a 0x30-byte record at `0x03004C30 + 0x30 ×
bg`: `+0x20` x, `+0x22` y in map pixels (BG2, the stage: `0x03004CB0`/`CB2`).
`Bg_UpdateScrollShadow` (`082158B0`) masks them to 8 bits into the shadow at `0x03004C20` (BG0-3 H/V,
eight halfwords), which `sub_08213FD8` copies to `0x04000010` in VBlank (found
with write watchpoints on `REG_BG2HOFS` and the shadow in headless mGBA).

### Stage scripts: doors, entrances, items, enemies

`tools/assets/stages.py show SCENE... | json [OUT]` walks each scene
procedure (`tools/mgba/scenes.py`, plus every door destination it lists) with
literal arguments substituted and writes `build/assets/stages.json` (429
scenes: 1,057 doors, 1,223 entrances, 185 items, 598 enemy spawners). Each
record keeps the conditions it sits under (`progress` = `r708[0x3c]`, the
story counter; `entrance` = `r708[0x70]`, the entrance used to arrive).
A scene's setup runs inside `if (proc_872() == 0)` and calls scene-local
procedures that use these library ones (call IDs; `gcx.py dis` prints one
less):

| Procedures | Arguments | Meaning |
|---|---|---|
| `884`, `885`, `876` | map, tileset, hp | load the stage (`876` is the loader the others wrap) |
| `888` | palette, 0, 0 | the stage's BG palette |
| `909`, `910`, `921`-`925` | box, facing, id[, proc] | entrance `id`: the player starts at HP box `box` |
| `911`-`913` | box, facing, ..., id | the same with more options |
| `914`-`918` | x, z, facing, ..., id | entrance at a position (`916`: x, z, facing, ?, ?, ?, id, proc, proc) |
| `919`, `920` | box, facing, id, ... | entrance at a door actor (`proc_961`, actor `0xA8F1`) |
| `934`-`938`, `949`, `950` | box, scene, entrance, back, ... | door: a trap on the box (`trap box 0xf5eb`) whose event loads `scene` at `entrance` |
| `940`-`945`, `951` | scene, entrance, ... | the same transition without a box, called from a `trap` event block |
| `946` | box, ?, ?, box, scene, entrance | a locked door (the first box opens it) |
| `1118` | category, item, x, z, facing, flags, flag | an item on the floor |
| `1119` | category, item, x, z, facing, chest, ?, flags, flag, proc | an item; `chest` = 1 puts it in a chest |
| `1496`-`1627` | name, path, ... | enemy spawners, one actor family each (`cmd 0xB745 KEY N: name r: path`) |

Entrances work like this: a door's event (`proc_953`) stores the entrance in
`r708[0x70]` and the door's fourth argument, the entrance that leads back to it
(the "8" of `proc_934 0xcd23 0xa91 57 8`, *guess*), in `GameState+0x740`;
`proc_952` starts the transition actor with the scene. The destination scene's
entrance procedures each test `if (r708[0x70] == id)` and the matching one
places the player (`proc_932` writes `GameState+0x30`/`+0x34`, gets the height
with `cmd 0xB745 0xCD4`, and the facing into `+0x36`). In the replay, the
first dungeon's door (scene 2698, box `CD23`) leads to scene 2705 entrance 57,
`proc_910 0xcd22 1 57`, the centre of box `CD22` at (640, 256, 2688), where
the game put the player.

Items: `proc_1120` creates either the pickup (`cmd 0xB745 0xDDCF I: category
item s: x y z`) or, for chests, actor `0x430D` whose event spawns it. The item
sits at the centre of its cell (x + 128, z + 128). `flags` low byte selects
the "already taken" flag table (1: 48 flags at `r708[0x13f]`, 2: 256 at
`r708[0x41]`) and `flag` the index. Category 0 items are named by strings
988 + item (#988 回復薬, #989 大地の実, ... #1001 太陽の実 = item 13; their
descriptions are 1043 + item); the first dungeon holds 太陽の実 (13) and
大地の実 (1). The other categories (1, 2, 4, 6, 8, 9) are not mapped to names
yet.

## The script image (`gcx`)

`tools/assets/gcx.py info | dis [PROC...] | chara` decodes it.
`sub_0821AE04` loads the image: a u32 timestamp (`0x42A05217`, 2005-06-03),
a procedure table of u32 words ending in `0xFFFFFFFF` (16,992 slots, 5,615
used; low 24 bits = offset from the body, top byte = a flag 0-14), then four
section offsets: the body (`08DD6BA0`, 0x884A4 bytes, prefixed by its size),
the string offset table (`08D6B1F8`) and string data (`08D74DDC`) that
`Text_LookupString` uses, and a fourth word before the body.

Instructions: the high nibble of the first byte is the kind and the low nibble
the payload length (13-15: a 1-3 byte little-endian length follows). Kind 6 is
a native command: u16 command id (a name hash, see ENGINE.md), then an
argument list with its own length (one byte below 0x80, else two big-endian
bytes & 0x7FFF, `sub_0821A6C0`) holding operands. Kind 7 calls a procedure,
kind 3 is an expression, kind 0 ends a block. Operands follow
`Script_DecodeOperand`; nested `0x8X` blocks hold `if`/`switch` bodies.
Expressions (`Script_EvalExpression`) are postfix: operands are pushed and a byte
`0xA0 | op` applies operator `op` (`Script_ApplyOperator`: 1 neg, 2 not, 3 invert,
4 `+`, 5 `-`, 6 `*`, 7 `/`, 8 `%`, 9 `<<`, 10 `>>`, 11-16 comparisons, 17 `|`,
18 `&`, 19 `^`, 20 `||`, 21 `&&`, 22 assign, 23 right operand); `0xA0` ends it.
`gcx.py` prints them infix. A command's argument list may be followed by
keyword items (tag `0x5X`, letter, values): `if` has `e` (else) and `i` (else
if) blocks, `switch` has `c` (case value, block) and `d` (default), and other
commands take named parameters this way (`p:` position, `e:` event block, ...).

**`chara` usage.** The procedures contain 1,525 `chara` commands naming 151
distinct registry keys, all present in `data/actor_registry.csv` (for example
`0xB4B4` = `Background_CreateFromScript`). The remaining 571 registry entries
are not named by any script constant. Only three code sites call the registry
lookup, and map files show no registry keys above chance, so those entries are
created another way (per-stage data not yet decoded) or are unused; the ROM
also keeps Boktai 2 material (its title graphics in `E89F`). The procedure
table covers the whole body, so no script code is unreachable from it.

## Text and the font

`tools/assets/text.py dump | find WORD` decodes the script string table
(9,977 entries at `08D6B1F8`, base `08D74DDC`; bit 31 of an entry marks text).
Bytes below `0x80` are ASCII (markup such as `<PROC=0>`, `<LABEL>`, `<ALTER>`,
newlines); a lead byte `0x80`-`0x85` and the next byte form a glyph index
(code - `0x8000`).

The font (`fnt` resource) has a 12-byte header (`u16 0x80` ASCII glyphs, `u8 1`,
`u8 6`, `u32 0xC` ASCII offset, `u32 0x200C` two-byte offset), 128 ASCII glyphs
of 8x16 and 1,537 two-byte glyphs of 16x16, 4bpp as four 8x8 tiles (TL, TR,
BL, BR); colour 4 is background, 1 the stroke and 2-3 shading. Kana and
symbols come first (`0x001`-`0x0FF`), then kanji grouped by radical, with
"PG" logo glyphs at `0x100`, `0x200`, ... `0x600`.

**Cross-checked against the fan translation** (`moozilla/boktai3trans`,
`scriptEd/`): its `chartable.sjs.tbl` settled 25 glyphs in its favour (for
example 聞 for 閣 in 聞く, 秘, 運, 障, 貨/貸 swapped, 讐, 誓, 必, 風), each
confirmed by the strings' context; 冠 stays ours (妖精の冠(かんむり)).
After the fixes, 7,098 of the 7,217 strings whose Japanese its `script.sjs`
keeps as comments decode identically; the rest differ only in notation
(`{note}` for ♪, `{＋}` for ✚, 〜 for ～) or stray English in a comment. Its
`protected_zones.txt` lists the same 82 binary entries as `text_bank`.

Glyph order is not Shift-JIS order. In the kanji section (`0x100` on), only
526 of 1,073 neighbouring mapped pairs increase in SJIS code, about chance,
while the runs follow radicals (亻: 仲 何 付 代…, 彳: 行 役 彼 後…, 扌: 打 払 折…).
Monotonicity therefore cannot validate the map; cross-checking against a
translation project's table or translated strings can.

`data/charmap.tsv` maps glyph indices to characters (1,326 entries). Kana and
symbols were read by eye; kanji were ranked by template matching against
Hiragino Kaku Gothic W7 at the glyph's ink box (IoU of blurred bitmaps, JIS
level 1) and each confirmed or corrected by eye (`confidence` column: `high`,
`match`, `low`). Every two-byte code used by the strings is mapped. Decoding
exposed two misreads that context fixed (嗅 in 嗅覚, 冑 in 甲冑); expect a few
more among the `low` entries.

What the text shows: item, weapon, armour and bike-part names; the character
list (#9818-#9833, including the cameos スネーク, リキッド, ソリダス); and the
monster list (#9838-#9921: ポイズナー ... グール, ゾンビ, マミー, スケルトン,
ゴーレム, ミミック, the elemental armours, スライム, 伯爵, and the bosses
フレスベルグ, サバタ, ニーズホッグ, ラタトスク, ヴァナルガンド), followed by drop
labels, elements (#9927-#9935), monster classes (ビースト, アンデッド, シング,
ファントム, イモータル) and dungeon descriptions. Linking these names to the
actor code (which kind is which monster) is the next step.

## Large sprites (type `98F5`)

`tools/assets/bigsprites.py list | index | sheet HASH [--palfile F]`. The
single 1.7 MB entry holds 381 sprites: enemies, props (doors, chests, trees,
traps) and boss parts. Actor code requests them by name with
`ScriptProc_Start(0xCB05, name)` even though they are not in the `CB05`
archive (for example `9247`, a shambling humanoid, from `sub_0808D1D8`), so
the `CB05` lookup must also search this resource; the exact path is not yet
traced. Layout (inferred): `+0x04` count, `+0x0C` pixel data start, `+0x10`
frame table, `+0x14` part table; at `+0x18`, records of 12 bytes
`{u32 name hash, u8 w, u8 h, u8 centre x, u8 centre y, u32 frame offset}`;
frames of 12 bytes `{u8 parts, u8 tiles, u16 flags, u32 pixel offset,
u32 part offset}`; parts of 4 bytes `{u8 size << 2 | shape, u8 flags, s8 x,
s8 y}`, whose tiles follow each other in the frame's 4bpp pixel block.
Matching these sprites to the monster names and actor kinds is open.

### Enemy families and their sprites (tentative)

Each enemy family in `0x08084F44`-`0x080FE68C` loads one large sprite by
name. The identifications below are visual guesses from renders with a
borrowed palette, matched against the monster list in the text; treat them as
leads for naming, not facts.

| Loader | Registry key (spawner) | Sprite | Frames | Looks like | Candidate monster |
|---|---|---|---|---|---|
| `sub_08084F44` | `F8BE` (`sub_08085D20`) | `DA6D` | 16 | small black wings | バット / ヴァンパイアバット |
| `sub_0808D1D8` | `05A5` (`sub_0808E1D4`) | `9247` | 12 | hunched shambling humanoid | グール / ゾンビ |
| `sub_08090FEC` | `C3BC` (`sub_08091E1C`) | `B96C` | 16 | dome with an eye | スライム |
| `sub_08094764` | `E131` (`sub_08095540`) | `9506` | 29 | small winged insect | ビー |
| `sub_08099B44` | `CCD3` (`sub_0809ADD4`) | `D637` | 29 | hooded cloaked figure | ? |
| `sub_0809DD78` | `680E` (`sub_0809ECC8`) | `D28E` | 42 | small flame | クロロホルルン / モエルルン / ヒエルルン (Klorofolun, Burrnun, Chillun)? Enemy kinds 26-28; rendered in captured colors it is a flame, not a ring |
| `sub_080A1440` | `9E8D` (`sub_080A2010`) | `5A6E` | 23 | black bird | クロウ / レイブン |
| `sub_080A4AFC` | `87E4` (`sub_080A5B54`) | `528F` | 16 | spider | スパイダー |
| `sub_080ABDCC` | `0AC7` (`sub_080ACBA8`) | `D636` | 44 | round body with tentacles | オクトパス? |
| `sub_080CFC14` | `98EC` (`sub_080D0BFC`) | `5290` | 47 | round red/orange creature | ? |
| `sub_080DCC98` | `6913` (`sub_080DDCF4`) | `48DE` | 25 | thin green figure | ? |
| `sub_080E5744` | `F836` (`sub_080E6814`) | `D635` | 64 | small dragon | ドラゴンパピー? |
| `sub_080E87C4` | `BEF5` (`sub_080E986C`) | `3876` | 8 | arcade cabinet | ? |
| `sub_080EC3F8` | `3F99` (`sub_080ED2F0`) | `D291` | 35 | flame worm | モエルルン? |
| `sub_080F0C20` | `1744` (`sub_080F1BD4`) | `D638` | 37 | pale worm | アイビー (Ivy): proc 3278 places "ivy as destroyed remains" with D638 |
| `sub_080FE68C` | `2D30` (`sub_080FF754`) | `5291` | 4 | living sword | ソード |

Each family is spawned through the script actor registry
(`data/actor_registry.csv`): the key's callback calls the family loader. The
small-sprite families are `DFDC` (`sub_080B54A8`, black wings) and `AD03`
(`sub_080C44AC`, skeletons). Registry names are not the sprite names with a
shared prefix or suffix (no affix of up to five characters maps both `slime`
to `C3BC` and `bee` to `E131`), and about 300,000 English, romanised and
decorated spellings tested against each family's own key hit only at the
chance rate.

In the late-game save's field scene (`tests/save_menus_field.txt`), the only
family code that runs is `sub_0809DD78` (`D28E`, "ring"), alongside a red ring
drawn around Django, so that family may be a field effect rather than an enemy.

**With real colours (stage survey, 2026-10-14).** Warping through every scene
(docs/CENTAUR.md) captured the in-game palettes of twelve families. Seen in
colour, several earlier guesses change:

| Sprite | In colour | Revised candidate |
|---|---|---|
| `DA6D` | small purple bat | バット |
| `9247` | green-headed shambling humanoid | グール / ゾンビ |
| `B96C` | red slime (name hash `slime`) | レッドスライム (colour variants share the sprite) |
| `D637` | hooded figure, pink robe, cyan hands, one eye | レイス? |
| `D28E` | small flame, not a ring | a flame or will-o'-wisp effect; runs in the save's field |
| `48DE` | blue bandaged figure | マミー |
| `5290` | brown rock/clay body with fists | クレイゴーレム / ゴーレム |
| `D635` | pink four-legged dog | ハウンド (not a dragon puppy) |
| `3876` | treasure chest that opens | ミミック (not an arcade cabinet) |
| `D638` | green spotted worm in a burrow | クロロホルルン |

The black wings `3DB6` and skeletons `AE52` (small-sprite packages) were also
captured. Palettes per sprite are kept in `build/centaur/sprite_palettes.json`
and used by the studio.

Sprite names are string hashes (`sub_08219BD8`, see [DATA.md](DATA.md)) of
plain English words in two cases: `slime` = `B96C` and `bee` = `9506`, matching
the visual identifications above. Romanised Japanese and English guesses for
the other families gave nothing above chance; with 1,500 resource hashes, a
random word hits one about 2% of the time, so only a family-level guess
confirmed by the render counts.

Two families load small-sprite packages instead: `sub_080B43BC` (`3DB6`, a
black winged creature) and `sub_080C3260` (`AE52`, skeletons).

**The game's own enemy names (2026-10-15, partial).** The text bank holds a
monster book: names at 9838-9921 (in the order DarthNemesis's guide numbers
monsters), descriptions from 2509, then `通常:`/`レア:` (9924/9925, drops),
elements (9926 `---`, 9927-9935) and classes (9936-9940 ビースト, アンデッド,
シング, ファントム, イモータル). No code adds 9838 to an enemy kind: code
reaches strings as a script-supplied base (`Script_ParseStringRef`, the base
kept in the object) plus an offset, and no script operand gives a base near
9838; the `0`/`2` entries beside the lists (9834-9837, 9922, 9941-9951) are
`B` data records, not bases. The late-game save's pause menus never call
`Text_LookupString` in that range. Enemy families are created by
`cmd_B745 KEY` in spawner procedures (flags 5 and 6) that no script calls, so
the engine or stage data starts them, probably with the variant (ghoul,
zombie, ghoul+) among their seven arguments. The studio's Actors page links
families to names through the sprite identifications above
(`tools/assets/actors.py`, `SPRITE_NAMES`), as candidates.
