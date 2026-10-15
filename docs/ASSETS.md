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
| `AE6C` | ? | `087C4C98` | 528 | LZ77; decompressed header magic `HP` |
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
actor that draws it; which actor field holds it is the next step (watch a
run with the enemy on screen, e.g. the ghoul `9247`, pool entry 403).

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
(LZ77, magic `HP`), probably collision or height data (not decoded). The
field stage of the save is `proc_884 0xcf70 0xf8ea 0x7239` (procedures 1491,
3241, 6388): map `CF70` (32 × 48 metatiles), tileset `F8EA` (351 tiles), BG
palette `92B3:E39B` banks 0-6 (matched against palette RAM; the script does
not name it, so the palette is chosen elsewhere, perhaps by area or time of
day). The OBJ palette banks 0-5 of that scene match no palette resource, so
sprite colours are assembled at runtime.

The studio's Maps view renders every stage and map this way
(`tools/studio/serve.py`, `/gfx#maps`).

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
| `sub_0809DD78` | `680E` (`sub_0809ECC8`) | `D28E` | 42 | small ring-like | ? |
| `sub_080A1440` | `9E8D` (`sub_080A2010`) | `5A6E` | 23 | black bird | クロウ / レイブン |
| `sub_080A4AFC` | `87E4` (`sub_080A5B54`) | `528F` | 16 | spider | スパイダー |
| `sub_080ABDCC` | `0AC7` (`sub_080ACBA8`) | `D636` | 44 | round body with tentacles | オクトパス? |
| `sub_080CFC14` | `98EC` (`sub_080D0BFC`) | `5290` | 47 | round red/orange creature | ? |
| `sub_080DCC98` | `6913` (`sub_080DDCF4`) | `48DE` | 25 | thin green figure | ? |
| `sub_080E5744` | `F836` (`sub_080E6814`) | `D635` | 64 | small dragon | ドラゴンパピー? |
| `sub_080E87C4` | `BEF5` (`sub_080E986C`) | `3876` | 8 | arcade cabinet | ? |
| `sub_080EC3F8` | `3F99` (`sub_080ED2F0`) | `D291` | 35 | flame worm | モエルルン? |
| `sub_080F0C20` | `1744` (`sub_080F1BD4`) | `D638` | 37 | pale worm | ヒエルルン / クロロホルルン? |
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
