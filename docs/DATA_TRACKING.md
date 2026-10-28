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
PROGRESS.md and the decomp.dev report), 2026-10-27:

| ROM data region `0824DAFC–08E88144` | Bytes | Matched | Identified |
|---|---:|---:|---:|
| **all** | 12,822,088 | **3.48%** (446,538) | 99.1% |
| Graphics | 7,096,380 | 0.0% | 100.0% |
| Palettes | 135,524 | 0.0% | 100.0% |
| Sound | 3,876,396 | 0.0% | 99.4% |
| Text | 440,754 | 100.0% | 100.0% |
| Tables (code rodata) | 85,156 | 6.8% (the actor registry) | 6.8% |
| Scripts (GCL) | 794,422 | 0.0% | 100.0% |
| Other/unknown | 393,456 | 0.0% | 97.3% |

How the build emits the region today: decoded source 3.5%, extracted resource
files 59.5% (opaque `.bin`, matched 0), `.incbin` of the base ROM 37.0%. Named
data symbols (`symbols/data.csv`): 10. The Studio's own levels (needs the ROM):
96.1% decoded or named, 28.5% named; the inventory's 99.1% "identified" adds what
the Studio view leaves out (archive headers, `HP` terrain, which `hp.py` decodes).

## 4. The report (implemented 2026-10-27)

The owner's rules: the headline data measure is ROM data; RAM leaves the
decomp.dev report; "matched" is strict; one data-only category per kind.

### Matched: every byte understood and rebuilt from an editable, meaningful source

A byte is **matched** only when the build emits it from a source a person can
read and edit, and that source rebuilds it byte-exact. The format choice is
flexible; the principle is not:

| Kind | Matched when built from |
|---|---|
| Graphics (tiles, sprites, maps, fonts, animations) | PNG plus JSON metadata that round-trip byte-exact (compression included) |
| Palettes | JSON or `.pal` |
| Sound | pret-style m4a song `.s` files; voicegroups and the song table as source; samples as WAV/AIF |
| Text | the decoded text bank (`text_bank`, as now) |
| Tables (code `.rodata`) | typed C `const` arrays in the C units that use them (or an equally typed table source; the actor registry's CSV of keys and named callbacks counts) |
| Scripts (GCL) | GCL source, fully annotated (below) |
| Other/unknown | a decoded format of its own, as for the kinds above |

Archives and opaque `.bin` extracts count **0** until every entry is matched
under its own kind: extracting bytes into a file is not understanding them. So
the 59.5% the archives emit from extracted files is not matched.

**Identified** (the format is known: a tool or the Studio decodes it) is a
PROGRESS.md number, not a match, the analog of a function boundary with assembly.

`complete_data` equals `matched_data`: everything is linked into every build.

**Scripts (GCL).** The unit of annotation is the **procedure**: procedure ids are
stable table slots (`gcl/README.md`), each procedure has a byte extent in the
bytecode, and names are kept per procedure (`gcl/names/proc.csv`, `param.csv`).
Per script file is too coarse (files are inferred groupings) and per byte too
fine to annotate. A procedure is

* **decompiled** when the build compiles its GCL source to its exact bytes;
* **matched** when it is decompiled *and* annotated: the procedure is named
  (`names/proc.csv`), every procedure it calls or references is named, every
  `chara`/`command` key it uses has a name (`names/chara.csv`, `command.csv`), the
  variables it reads or writes are named (`names/var.csv`), and nothing unknown is
  left: no unknown opcode, no raw-byte escape, no `unk`/`proc_NNNN` label.

The GCL compiler (not written yet; `gcl/src/` is the planned source) records the
status per procedure in a committed `gcl/progress.csv` (`proc,start,size,status`,
status `decompiled` or `matched`; addresses and sizes only, no script content), so
the measure stays ROM-free like the function inventory. `tools/romdata.py`
credits `matched` procedures' bytes and reports `decompiled` ones as a separate
PROGRESS.md number. The procedure and pointer tables around the bytecode are to be
credited once every procedure is matched (they are generated from the table of
procedures); that rule waits for the compiler. Today no GCL is compiled, so Scripts is 0 matched, 0 decompiled.

### The inventory and the report

`symbols/rom_data.csv` (`start,end,kind,unit,identified,evidence`) splits the data
region into 20 contiguous units, each of one kind; `tools/romdata.py` checks that
they tile the region exactly. Matched bytes are computed, never typed: the
overlap of a unit with `data/manifest.csv` ranges whose datafmt module declares
`SOURCE = "decoded"`, plus `matched` GCL procedures. `identified` is a reviewed
byte count (from the Studio's ROM view and the decoders in `tools/assets/`).

`tools/progress.py --json` (`add_rom_data`):

* headline `total_data`/`matched_data`/`complete_data`: ROM data only;
* one category per kind, ids `rom_graphics`, `rom_palettes`, `rom_sound`,
  `rom_text`, `rom_tables`, `rom_scripts`, `rom_other`, named `ROM data: Graphics`
  and so on; their totals sum to the headline;
* one unit per inventory row, named `ROM data: KIND/UNIT`, in exactly one of those
  categories, with data measures only (code fields 0) and one `.rodata` section
  item (size, virtual address, matched percent);
* code units and code categories carry no data; the headline unit counts
  (`total_units`, `complete_units`) stay those of the code, so the code numbers on
  decomp.dev do not move.

RAM (`symbols/ram.csv`) stays in PROGRESS.md and the summary as before and is no
longer in the report. PROGRESS.json (the claude-mod tree) leaves out the data
units, which have no functions.

### Next steps

1. **Code `.rodata` into C** (2.3): move the 11 KB of tables after the code into
   the C units that read them as typed `const` arrays. Needs the link to place
   each object's `.rodata` in ROM order after all `.text` (`*(.rodata)` in a
   second output pass) and the asm units' slices of `0824DAFC–08250674` to follow
   the same order. Then a C unit's `.rodata` matches or not, as objdiff measures it.
2. **Palettes** (135 KB, simplest): `92B3` and the `9B1B` pool as JSON/`.pal`
   with a datafmt module (`SOURCE = "decoded"`).
3. **Sound** (3.9 MB): pret-style song `.s`, voicegroups and the song table as
   source, samples as WAV/AIF (`tools/m4a.py` already walks all of it).
4. **Graphics** (7.1 MB): an LZ77 compressor that reproduces the original bytes
   (try pret's `gbagfx` first), then maps, tilesets and sprite sheets as PNG plus
   JSON. Split the archive units by entry when entries start to match.
5. **GCL**: a compiler from `gcl/src/` and `gcl/progress.csv` (above).
6. **objdiff data diffs**: once data lives in `.rodata` sections with sized
   symbols, an `objdiff.json` can pair a target object per data unit (assembled
   from the original `.incbin` slices) with the base object built from source,
   and objdiff shows the hex diff per table. Until then `tools/datafmt/check.py`
   is the byte-exact check.

## Studio fix (2026-10-27)

`atlas.py speaker_portraits()` looked for `thumb_func_start sub_0803232C` in
`gen/code.s`; the function has been renamed (`TextState_ProcessTag`), so the
Studio index (and with it the ROM view) failed to build. It now takes the current
name from `symbols/functions.csv`.
