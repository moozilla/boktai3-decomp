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
"data" measures count today.

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

From `python3 tools/progress.py` (ROM-free, also in PROGRESS.md):

| ROM data region `0824DAFC–08E88144` | Bytes | Share |
|---|---:|---:|
| total | 12,822,088 | 100% |
| emitted from decoded source (text bank, actor registry) | 446,538 | **3.5%** |
| emitted from extracted resource files (archives) | 7,625,360 | 59.5% |
| `.incbin` of the base ROM, pointer words as symbols (sound, tables, bytecode) | 4,750,190 | 37.0% |
| named data symbols (`symbols/data.csv`) | 10 | |
| Studio: format decoded (named or decoded level; needs the ROM, not in PROGRESS.md) | 12,316,745 | 96.1% |
| Studio: format decoded and named | 3,651,846 | 28.5% |

RAM, for comparison (the current decomp.dev data measure): 4,695 of 16,077 bytes
(29.2%) with one shared declaration.

## 4. The plan

### What counts as matched ROM data

Two levels, mirroring code (matched C vs assembly), at byte granularity:

1. **matched**: bytes emitted from a **decoded source** that rebuilds them
   byte-exact: a datafmt range with `SOURCE = "decoded"`, or later a `const`
   array in a C unit's `.rodata`, or an asset converted from an editable file
   (PNG, JSON, `.s` song) that round-trips. Opaque `.bin` extracts don't count,
   any more than `.incbin` does: they are the ROM's bytes in another file.
2. **identified** (reported as a category, not as matched): bytes inside a
   located object with a known format: split archive entries, and Studio
   "decoded" blocks the build does not yet emit from source. This is the analog of
   a function boundary with assembly: progress toward matching, not a match.

Names are tracked separately, like function names: a named data symbol (from
`symbols/data.csv` or a datafmt anchor) is a count, not a byte credit. Pointer
symbolization (relocatability) is a build property, already complete (the shift
test), and not progress by itself.

`complete_data` (objdiff's "linked") would be the same as matched: everything is
linked into the ROM in every build.

### Keeping the RAM measure distinct

objdiff's own definition puts RAM (`.bss`) and ROM constants (`.rodata`) in the
same `total_data`. Adding 12.8 MB of ROM to 16 KB of RAM would bury the RAM
measure, so report both separately, each a **report category** with only data
measures:

* `ram` (**RAM objects**): today's `symbols/ram.csv` accounting, unchanged;
* `rom_data` (**ROM data**): the matched bytes above;

and choose which one the headline `total_data`/`matched_data` carries. The
recommendation: the headline becomes **ROM data**, because that is what other
projects' data numbers mean to a reader (graphics, tables, text decompiled into
source), and RAM stays one click away as its category. The alternative, keeping
RAM in the headline, is also consistent; it is the owner's call. Either way the
units must carry the measures too, since objdiff/decomp.dev expect a category's
totals to be the sum of its units:

* add data units named `ROM data/<region>` (for example `ROM data/Text bank`,
  `ROM data/Maps (C091)`, `ROM data/Sound`, `ROM data/Code rodata`), one per
  manifest range or ROM_MAP region, with `progress_categories: ["rom_data"]`,
  `total_data`/`matched_data`, and one `sections` item each (`.rodata`, size,
  fuzzy percent = the matched share);
* add one unit `RAM/objects` with `progress_categories: ["ram"]`, holding the RAM
  measures that today exist only at the top level;
* leave every code unit's data fields at 0, so the code categories are unaffected.

Check after the first such report that decomp.dev shows a data-only category and
data-only units sensibly (it already renders a report whose units all have
`total_data` 0).

### Steps

1. **Done (this change):** `progress.py` computes the ROM data numbers from
   `data/manifest.csv`, the datafmt `SOURCE` declarations and `symbols/data.csv`
   (`rom_data_measures()`, ROM-free, unit-tested) and prints them in PROGRESS.md
   and the summary. The decomp.dev report is unchanged.
2. **Report it:** the categories and units above in `make_report`, plus the
   headline choice. Small; needs the owner's decision on the headline.
3. **Data inventory:** a `symbols/rom_data.csv` (address, size, name, format,
   status), seeded from what already exists: the label extents in `gen/data.s`,
   the Studio's `rom_tree()` blocks (archives by entry, sound by song and sample,
   tilesets, large sprites), the ROM_MAP regions. ROM-free in CI like the function
   inventory. It gives the report one item per data object, the Studio one source
   of truth, and `symbolize.py` sizes for its labels (`.size`, `.type @object`).
4. **Code `.rodata` into C** (2.3): move the 11 KB of tables after the code into
   the C units that read them. Needs the link to place each object's `.rodata` in
   ROM order after all `.text` (`*(.rodata)` in a second output pass) and the
   asm units' slices of `0824DAFC–08250674` to follow the same order. Then a C
   unit's `.rodata` matches or not, exactly like objdiff measures it.
5. **More decoded formats**, largest first, each a datafmt module with
   `SOURCE = "decoded"` that round-trips: palettes (`92B3`, the `9B1B` pool) as
   JSON/`.pal`; MP2K songs, voicegroups and the song table as pret-style `.s` and
   CSV (3.9 MB with the samples as `.bin`/`.aif`, which are already their own
   format); the archive entries' LZ77 (needs a compressor that reproduces the
   original bytes; try pret's `gbagfx` first) and then maps, terrain and tile
   sheets as PNG/JSON; the event bytecode from the GCL sources (`gcl/`).
6. **objdiff data diffs**: once data lives in `.rodata` sections with sized
   symbols (steps 3-4), an `objdiff.json` can pair a target object per data unit
   (assembled from the original `.incbin` slices) with the base object (built from
   source) and objdiff shows the hex diff per table. Until then
   `tools/datafmt/check.py` is the byte-exact check, and the report is written by
   `progress.py` as now.

## Studio fix in the same change

`atlas.py speaker_portraits()` looked for `thumb_func_start sub_0803232C` in
`gen/code.s`; the function has been renamed (`TextState_ProcessTag`), so the
Studio index (and with it the ROM view) failed to build. It now takes the current
name from `symbols/functions.csv`.
