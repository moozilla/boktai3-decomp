# Tracking ROM data like code

Tags: #data #progress #process

How decomp.dev and objdiff measure data, what this project has for the ROM's
non-code bytes, how much of them is identified today, and a plan to report ROM
data progress and show data diffs. Research of 2026-10-27; numbers below are from
`main` at `e8f4b9d4d`. The current figures are always in [PROGRESS.md](../PROGRESS.md)
(`python3 tools/progress.py`).

ROM data here means everything in the cartridge that is not code: tables,
graphics, maps, fonts, text, scripts and sound. It is a different thing from the
**RAM** inventory (`symbols/ram.csv`, [RAM.md](RAM.md)), which decomp.dev's
"data" measures counted until 2026-10-27. Since then the report's data measures
are ROM data only (section 4), and RAM is tracked in PROGRESS.md alone.

## 1. How objdiff and decomp.dev measure data

Sources: objdiff `objdiff-core/protos/report.proto` and
`objdiff-cli/src/cmd/report.rs` (main at `9aeec9cb`, 2026-10-19), and
`objdiff-core/src/diff/data.rs`; the project page on decomp.dev.

**The report.** `Measures` has three data fields next to the code ones:
`total_data`, `matched_data` (+ `_percent`) and `complete_data` (+ `_percent`).
They exist at three levels: the whole report, each `ReportCategory`, and each
`ReportUnit`. A unit also has `sections`: one `ReportItem` per section (name,
size, `fuzzy_match_percent`, virtual address). Functions are listed per unit;
data symbols are not listed in the report at all.

**How `objdiff-cli report generate` fills them.** For each unit it reads the
target object (built from the original) and the base object (built from source)
and diffs them. Then for every section:

* a section of kind **Data** (`.data`, `.rodata`, `.sdata` and similar) or **Bss**
  adds its whole size to `total_data`, and adds it to `matched_data` only when
  the section matches **100%**. A section that is 99% right counts zero matched
  bytes; there is no per-symbol data credit.
* a **Text** section is counted function by function (`total_code`,
  `matched_code`, fuzzy percent weighted by size).
* `complete_data = total_data` when the unit is marked complete (linked), else 0.

So in objdiff's terms "data" is `.data` + `.rodata` + `.bss`: initialized ROM
constants and RAM alike. A data section's match percent is a byte diff of the
section (up to its last symbol's end) plus a relocation check: a pointer word
matches when it points at the same symbol name and addend, so moved data still
matches.

**What it needs from a project.** Objects with real sections: data in a section
named `.rodata`/`.data` (data in `.text` is diffed as code), symbols with sizes
(`.size`, `.type @object`) so the GUI can show one diff per table, and
relocations for pointers. In the objdiff GUI the data diff is a hex view per data
symbol with differing bytes and relocations highlighted.

**decomp.dev** reads the objdiff v2 JSON (`jp_report` artifact,
[PROGRESS_CI.md](PROGRESS_CI.md)). The project page offers **Code** and **Data**
as two measures (it shows "Code (2.41 MB)" and "Data (25.65 kB)" as the choices)
and the category filter. Selecting Data shows `matched_data` over `total_data` for
the selection. It takes the numbers as given; it never sees objects. Our report is
written by `tools/progress.py`, not by objdiff-cli, so we decide what the data
fields mean, as long as the meaning is stated and stable.

## 2. What we have

### How the build emits ROM data

All ROM data comes from one generated file, `gen/data.s` (`tools/symbolize.py`),
assembled into one object (`build/asm/data_g.o`, section `.text`) and linked
after the code. No C file defines ROM data: no `src/*.c` has a `const` table, and
the link script takes only `.text` from each object. C reaches data through
`extern const u8 gUnk_XXXXXXXX[];` labels (807 distinct ROM data labels are
referenced from C today; `tools/build.py` refuses raw ROM addresses).

`gen/data.s` covers `0824DAFC–09000000` (14,361,860 bytes, 1.54 MB of it the zero
padding after `08E88144`). Its lines are of three kinds:

| What | Bytes | Share of the data region |
|---|---:|---:|
| `.incbin "baserom.gba"` slices | 4,620,274 + padding | 36.0% |
| `.4byte SYMBOL`: ROM pointers written as symbols (32,479 words outside the manifest ranges) | 129,916 | 1.0% |
| emitted from `data/manifest.csv` sources (below) | 8,071,898 | 63.0% |

and **20,189 labels**, of which **5** have names (`gSongTable`, `gScriptTextPtrs`,
`gScriptText`, `gEventBytecode`, `gEventBytecodePtrs`); the rest are
`gUnk_ADDR`, created where code or a pointer references the address. Labels have
no size, so a label is a start, not an object.

**Decoded ranges** (`data/manifest.csv`, [DATA.md](DATA.md)): a format module in
`tools/datafmt/` decodes a range to a source file and encodes it back
byte-exact; `symbolize.py` emits the range from that file instead of
`.incbin` slices.

| Format | Ranges | Bytes | Source | Kind |
|---|---|---:|---|---|
| `keyed_callbacks` | the actor registry `08603300–08604998` | 5,784 | `data/actor_registry.csv` (committed) | decoded |
| `text_bank` | `08D6B1E4–08DD6B96` | 440,754 | `build/text/bank.tsv` (extracted) | decoded |
| `resource_archive` | 10 archives `08614DCC–08D5A85C` | 7,625,360 | `build/res/LABEL/index.txt` + one `HASH.bin` per entry (extracted) | split |

"Decoded" means the source is an editable form (CSV rows with named callbacks,
text strings). "Split" means the container (count, hash table, offsets) is
regenerated and each entry is its own file, but the entries are opaque bytes
copied out of the ROM: an LZ77 map is still LZ77, a palette is still raw words.
Each format module now declares which (`SOURCE = "decoded"` or `"split"`).

**`symbols/data.csv`** names 13 ROM objects (10 in the data region), five with a
size. It is a hand list from early mapping, not an inventory.

The ROM regions not covered by a manifest range:

| Range | Bytes | Contents ([ROM_MAP.md](ROM_MAP.md)) |
|---|---:|---|
| `0824DAFC–08603300` | 3,889,156 | 11 KB constant tables (see 2.3), then MP2K sound: voicegroups, song table, 415 PCM samples, 5,525 track streams |
| `08604998–08614DCC` | 66,612 | pointer tables, Thumb callback tables (actor/state dispatch) |
| `08D5A85C–08D6B1E4` | 67,976 | `Script_ReadProcTable` tables, palettes |
| `08DD6B96–08E88144` | 726,446 | event bytecode (GCL, the `gcx` image) and its pointer tables |

### What the Studio knows

The Studio (`tools/studio/serve.py`, ROM view `/rom`, built by
`tools/studio/atlas.py rom_tree()`) splits the whole ROM into blocks in address
order (functions by subsystem, each archive by resource, sound by song, sample and
voicegroup, the tileset library by tileset, the large sprites by sprite) and gives
each block an understanding level:

* **named**: decoded and given a meaning (a labeled map, palette, sprite, song title, tileset);
* **decoded**: the format is understood and a tool renders or plays it
  (sprites, maps, tiles, palettes, fonts, animations, text, scripts, sound);
* **located**: the extent is known but not the format;
* **unmapped**: bytes no tool or document accounts for;
* **free**: the zero padding.

Over the data region (`0824DAFC–08E88144`, 12,822,088 bytes; computed with
`rom_tree()` on 2026-10-27, which takes about 17 minutes from a cold cache):

| Level | Bytes | Share | Largest parts |
|---|---:|---:|---|
| named | 3,651,846 | 28.5% | large sprites 1.73 MB, tilesets 1.59 MB, sprite packages 0.32 MB |
| decoded | 8,664,899 | 67.6% | sound 3.85 MB, maps 1.75 MB, sprite packages 1.21 MB, event bytecode 0.72 MB, text 0.44 MB, tilesets 0.23 MB, font 0.20 MB, palettes 0.10 MB |
| located | 500,931 | 3.9% | `HP` terrain 0.38 MB, pointer tables `0860xxxx` 76 KB, the tables after code 11 KB, sound tables 22 KB, the `CEAA` blob 11 KB |
| unmapped | 3,184 | 0.0% | one gap at `087C4C98` |

(1,228 bytes fall in gaps below the view's block threshold.) So the Studio already
accounts for about 96% of the data bytes with a known format, but its levels are
coarse: a region is "decoded" when its kind has a viewer (the event bytecode and
the `Script_ReadProcTable` tables count because their kind is "script"), and `HP`
terrain shows as "located" although `tools/assets/hp.py` decodes all 528 entries
(`ARCHIVE_KINDS` maps it to "collision", which is not in `DECODED`). It is a
view of understanding, not of what the build emits from source.

Decoders the Studio and `tools/assets/` have beyond the build's formats: sprite
packages, large sprites, `MP` maps (LZ77), `ptn` tilesets, `HP` terrain, palettes
and the sprite palette pool, fonts, animations, MP2K sound (`tools/m4a.py`,
`tools/studio/sound.py`), GCL scripts (`gcl/`). None of them is a build format:
they read the ROM, they don't emit it.

### 2.3 A finding: the C files' `.rodata` is still there

The 11 KB "misc constant tables after code" at `0824DAFC–08250674` are the
**`.rodata` of the game's C files**, in link order: the 293 function pointers in
it point at code in ascending order (`0801D208` from `0824DBD4`, `08063A20` from
`0824DF90`, `08182F0C` from `0824EF18`, … `082313CC` from `0825029C`), with only 7
backward steps, each inside one table. The linker put every object's `.rodata`
after all `.text`, object by object, the standard GNU ld layout. So the tables C
code reads (callback tables like the result tutorial's at `0824DBC0`, the speaker
names `TextState_ProcessTag` compares, small constant arrays) can move into the C
units that use them, as `const` arrays in C, and be checked like code. This is
exactly what objdiff calls a unit's `.rodata`.

## 3. Current numbers

From `python3 tools/progress.py` / `python3 tools/romdata.py` (ROM-free, in
PROGRESS.md and the decomp.dev report), 2026-10-28, after
`tools/data_roundtrip.py` verified every block its codecs support:

| ROM data region `0824DAFC–08E88144` | Bytes | Matched (round-trips) | Identified |
|---|---:|---:|---:|
| **all** (decomp.dev headline) | 12,822,088 | **74.79%** (9,589,097) | 99.1% |
| Graphics | 7,096,380 | 74.9% | 100.0% |
| Palettes | 135,524 | 100.0% | 100.0% |
| Sound | 3,876,396 | 85.3% | 99.4% |
| Text | 440,754 | 100.0% | 100.0% |
| Actors | 5,784 | 100.0% | 100.0% |
| Tables (code rodata) | 79,372 | 0.0% | 0.0% |
| Scripts (GCL) | 794,422 | 0.0% | 100.0% |
| Other/unknown | 393,456 | 97.3% | 97.3% |

All 1,680,000 bytes of LZ77 blocks (252 maps, 528 terrain entries) round-trip by
content, and the greedy compressor of `tools/lz77.py` (longest match, nearest
first, minimum displacement 2) reproduces every stored stream byte for byte, so
the game's own compressor was that algorithm.

How the build emits the region is a separate question: decoded source 3.5%,
extracted resource files 59.5%, `.incbin` 37.0%. The build does not have to use
the editable form for a byte to count.

## 4. The report (revised 2026-10-28)

The owner's rules: the headline data measure is ROM data; RAM leaves the
decomp.dev report and is tracked in PROGRESS.md; one data-only category per kind;
"matched" means the content round-trips through a project tool, verified.

### Matched: content round-trip, verified

A byte is **matched** when a project tool (tools/assets, tools/studio,
`tools/m4a.py`, datafmt, gcl) does both:

1. decodes it to an editable, meaningful form: PNG plus JSON metadata for pixels,
   JSON or `.pal` for palettes, WAV plus JSON for samples, JSON with named fields
   for structures, CSV with named fields for tables, text for the text bank;
2. re-encodes that form to the same bytes.

The build does not have to consume that form. A viewer is not enough: the form
must re-encode, so an edit loses nothing. Fields whose meaning is unknown stay as
raw named numbers in the JSON (`unknown`, `h0`), as in `data/` (DATA.md); a block
with bytes outside every known section (a nonzero gap) does not count. For LZ77
blocks the decompressed content must round-trip; whether recompression is
byte-exact is a separate number in PROGRESS.md (level `lz-exact` against
`lz-content`), not a requirement.

**Identified** (a decoder exists, no encoder) is a PROGRESS.md number, not a match.

### Verifying without the ROM in CI

`tools/data_roundtrip.py` (run locally with `build/venv/bin/python`, it needs the
ROM, numpy and Pillow) runs each codec on every block it supports: decode, write
the editable form (an in-memory PNG through Pillow, WAV through `wave`, JSON or CSV
text), read it back, encode, compare. Blocks that come back byte-exact go to
`symbols/data_roundtrip.csv` (`start,end,what,tool,level`): ranges and codec names
only, no ROM bytes and no content hashes. `tools/romdata.py` and
`tools/progress.py` read that manifest in CI. Re-run the checker after adding or
changing a codec, then regenerate PROGRESS.

| Codec (`tool`) | Blocks | Editable form |
|---|---|---|
| `datafmt/text_bank` | the text bank | `bank.tsv` (strings via `data/charmap.tsv`, blobs) |
| `datafmt/keyed_callbacks` | the actor registry | `data/actor_registry.csv` (key, callback name) |
| `datafmt/resource_archive` | the 10 archive containers | JSON entry list (name hash, offset) |
| `palette` | 195 `92B3` palettes, the `9B1B` pool | JSON: colors as `[r, g, b]` (+ the bit-15 flag), header words |
| `font` | the `A635` font | PNG sheets of the 128 8×16 and 1,537 16×16 glyphs + JSON header |
| `tileset` | the `CEEF` library | JSON tileset table and index lists + PNG of the 49,638-tile pool |
| `map` | 287 `MP` maps (252 LZ77) | JSON layers (unblocked grids), metatiles (tile, flips, bank) + PNG own tiles |
| `terrain` | 528 `HP` entries (LZ77) | JSON grid cells, boxes, paths, nav meshes |
| `sprite` | 46 `CB05` packages | JSON frames, animations, parts, keys + PNG tile sheet |
| `m4a` | 415 PCM and 16 wave samples, 81 voicegroups, the song table | WAV + JSON header; JSON voices (pret field names); CSV song table |

### Categories and units

`symbols/rom_data.csv` (`start,end,kind,unit,identified,evidence`) tiles the data
region with 20 units of one kind each (`romdata.py` checks the tiling). The report
(`progress.py --json`, `add_rom_data`):

* headline `total_data`/`matched_data`/`complete_data`: ROM data only (scripts included);
* one data-only category per kind: `rom_graphics`, `rom_palettes`, `rom_sound`,
  `rom_text`, `rom_actors`, `rom_tables`, `rom_scripts`, `rom_other`, named
  `ROM data: Graphics` and so on; their totals sum to the headline;
* one unit per inventory row, `ROM data: KIND/UNIT`, in exactly one category, with
  data measures only and one `.rodata` section item; code units carry no data, and
  the headline unit counts stay those of the code.

**Actors** is its own category: the actor registry (`08603300–08604998`, 722
key-to-constructor records) is the game's object type table. **Tables** is the
code's constant data: the C files' `.rodata` (2.3) and the pointer and dispatch
tables. They count as typed C `const` arrays in the units that use them, or as a
CSV with named, typed fields that round-trips.

**Scripts (GCL)** is a data category on decomp.dev (the UI has nowhere else for
it), but PROGRESS.md gives it its own section beside code and data: the owner sees
the scripts as code, separate from the game's C. Its bar stays higher, and it is
credited only from `gcl/progress.csv`, never from the data manifest. The unit is
the **procedure** (stable table slots, a byte extent each, names kept per procedure
in `gcl/names/`). A procedure is **matched** when it decompiles to GCL, recompiles
byte-exact *and* is named: the procedure, the procedures it references, its
`chara`/`command` keys and its variables have names, and no unknown opcode or
`unk`/`proc_NNNN` label is left. One that round-trips but is not fully named is
reported next to it ("round-trips but unnamed"), not matched. The GCL compiler
(planned `gcl/src/`) writes `gcl/progress.csv` (`proc,start,size,status`;
addresses and sizes only). There is none yet, so both numbers are 0.

### Gaps: a decoder but no encoder (next work, largest first)

| Bytes | Kind | Block | Decoder today | Missing |
|---:|---|---|---|---|
| 1,770,664 | Graphics | `98F5` large sprites (381) | `tools/assets/bigsprites.py`, the Studio | a codec: frame/part tables to JSON, pixels to PNG |
| 794,422 | Scripts | GCL bytecode and procedure tables | `tools/assets/gcx.py`, `gclview.py` | a GCL compiler from `gcl/src/`, then names |
| 545,736 | Sound | song headers and 5,525 track streams | `tools/m4a.py` walk, Studio player | an m4a `.s` disassembler and assembler (pret macros) |
| 79,372 | Tables | code `.rodata`, pointer and dispatch tables | none (located) | typed C arrays (2.3) or typed CSV |
| 23,717 | Sound | keysplit tables, the player table, voicegroup gaps | partly (`sound.py`) | JSON for keysplits and the player table |
| 10,664 | Other | the `CEAA` blob | none | a format |
| 8,416 | Graphics | `922E` animation scripts (75) | partly (portrait animation in `atlas.py`) | a decoder and codec |

### Next steps

1. Large sprites (1.77 MB): the biggest single gain.
2. GCL compiler: Scripts, the owner's second code base.
3. m4a song `.s` round-trip.
4. Code `.rodata` into C: move the tables after the code into the C units that
   read them as typed `const` arrays. The link must place each object's
   `.rodata` in ROM order after all `.text`.
5. objdiff data diffs: once data lives in `.rodata` sections with sized symbols,
   an `objdiff.json` can pair a target object per data unit with one built from
   source. Until then `tools/datafmt/check.py` and `tools/data_roundtrip.py` are
   the byte-exact checks.

## Studio fix (2026-10-27)

`atlas.py speaker_portraits()` looked for `thumb_func_start sub_0803232C` in
`gen/code.s`; the function has been renamed (`TextState_ProcessTag`), so the
Studio index (and with it the ROM view) failed to build. It now takes the current
name from `symbols/functions.csv`.
