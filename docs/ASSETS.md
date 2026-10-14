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
| `92B3` | ? | `088223C4` | 195 | palettes: 20-byte header + 16 × 16 colours (532 B each) |
| `9B1B` | ? | `08D530DC` | 1 | 30 KB blob |
| `98F5` | ? | `08A2D944` | 1 | 1.7 MB blob (`count 0x17D` header; nested format not decoded) |
| `A635` | `fnt` | `089F8ED8` | 1 | font |
| `A8D9` | `gcx` | `08D5A85C` | — | compiled scripts, a single image (MGS's GCL file extension); not an archive (below) |
| `AE6C` | ? | `087C4C98` | 528 | LZ77; decompressed header magic `HP` |
| `C091` | `map` | `08617084` | 287 | LZ77; decompressed header magic `MP` |
| `CB05` | ? | `08BDDE08` | 46 | sprite packages (below) |
| `CEEF` | `ptn` | `0883BDA8` | 1 | 1.8 MB pattern (tile) blob |
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
| `+0x00` | `u16 h0` (unknown), then counts: frames, animations, parts, keys, tiles |
| `+0x0C` | frames, 20 B: `u16 id, u16 parts, u32 ?, s16 x, s16 y, u16 w, u16 h, u32 part offset` |
| `+0x10` | animations, 8 B: `u16 id, u16 keys, u32 key offset` |
| `+0x14` | parts, 8 B: `u16 attr, s16 x, s16 y, u16 OAM attr2` |
| `+0x18` | keys, 4 B: `u16 frame index, u16 duration` |
| `+0x1C` | 4bpp tiles, stored as a sheet 16 tiles wide |

A part's `attr` is `size << 10 | shape << 8 | vflip << 3 | hflip << 2` (GBA OBJ
shape and size); its last word is OAM attribute 2 (tile in bits 0-9, priority
10-11, palette 12-15). An OBJ's tile rows are 16 tiles apart in the sheet.
Palettes are not in the package; the renders below borrow OBJ palette RAM from
a field scene (`dump 05000200:200` in the harness), so colours can be wrong.

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
(9,976 entries at `08D6B1F8`, base `08D74DDC`; bit 31 of an entry marks text).
Bytes below `0x80` are ASCII (markup such as `<PROC=0>`, `<LABEL>`, `<ALTER>`,
newlines); a lead byte `0x80`-`0x85` and the next byte form a glyph index
(code - `0x8000`).

The font (`fnt` resource) has a 12-byte header (`u16 0x80` ASCII glyphs, `u8 1`,
`u8 6`, `u32 0xC` ASCII offset, `u32 0x200C` two-byte offset), 128 ASCII glyphs
of 8x16 and 1,537 two-byte glyphs of 16x16, 4bpp as four 8x8 tiles (TL, TR,
BL, BR); colour 4 is background, 1 the stroke and 2-3 shading. Kana and
symbols come first (`0x001`-`0x0FF`), then kanji grouped by radical, with
"PG" logo glyphs at `0x100`, `0x200`, ... `0x600`.

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
