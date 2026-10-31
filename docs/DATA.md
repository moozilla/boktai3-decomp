# Decoded data

The goal is a ROM whose data region is rebuilt from editable source files, the
same way the code region is rebuilt from C. This page describes the pipeline,
the policy for what is committed, and the formats decoded so far.

## Pipeline

* `data/manifest.csv` lists decoded ranges: `name,start,end,format,file,description`.
* `tools/datafmt/<format>.py` decodes a range to a source file (`decode`) and
  encodes the file back to items (`encode`): sized integers, and ROM pointers
  that `tools/symbolize.py` writes as symbols, so decoded tables stay movable.
* `tools/symbolize.py` emits each manifest range from its source file instead
  of `.incbin` slices. It refuses a range whose labels fall inside an item, or
  that leaves a detected pointer word unencoded.
* `tools/datafmt/check.py` checks that every entry re-encodes to the original
  bytes; `check.py --extract NAME` (re)writes an entry's file from the ROM.
  `make` rebuilds `gen/data.s` when a data file changes, and the final SHA-1
  check is the end-to-end test. Run `make shifttest` after adding a format
  (passed for `keyed_callbacks`, 2026-10-14).

To add a format: write the module, add the manifest row, run
`check.py --extract NAME`, then `make` and `make shifttest`.

**Extracted (ROM-derived) sources.** A manifest file under `build/` is
extracted from the ROM automatically when it is missing, so creative assets
are editable without being committed. **Resizable formats** (`RESIZABLE = True`)
may encode to a different size; everything after them moves. Such a format
declares `ANCHORS`, the only interior addresses that may be referenced. Data
words pointing anywhere else inside the range are rejected as pointers (84
graphics words resembled pointers into the text bank), and pointer-like words
inside the range are not relocated (the format holds no ROM pointers). `make mod` builds
`build/mod.gba` from edited sources without the SHA-1 check, trimmed back to
16 MiB so EEPROM stays at `0x0D000000`.

The shift test moves all data together, so it cannot catch offsets that span a
resized range. Check a resized build in the emulator: the first attempt to
grow the text bank failed until the script image's section offsets, which span
the bank, were encoded with it.

## Policy

* **Committed:** structured tables whose content is code-like facts (keys,
  indices, counts, flags, pointers to code or data), as decomp projects commit
  data tables as source. Unknown fields stay as raw values in their columns;
  nothing is dropped, and decode/encode must be byte-exact.
* **Not committed:** graphics, palettes, maps, audio, text and any other
  creative asset. Those formats extract to the ignored `build/` tree from the
  user's own ROM, for viewers and editors to load.
* Names in decoded files are provisional unless backed by evidence, as in
  `symbols/`. Informational columns (for example a callback's current function
  name) are ignored by `encode`, so renames never break a build.

## Formats

| Format | Range | File | Notes |
|---|---|---|---|
| `keyed_callbacks` | `08603300`–`08604998` | `data/actor_registry.csv` | 722 sorted `(key, callback)` records and a zero sentinel. The script `chara` command binary-searches it (`08225844`) and calls the callback. |
| `callback_array` | Eleven ranges listed in `data/manifest.csv` | `data/actor_commands/*.csv` | Ordered Thumb callback pointers for the common actor commands and ten monster extensions. The command packet's first halfword selects an entry; every non-null pointer remains a linker relocation. |
| `actor_marker_table` | `0824E6D4`–`0824E6F8`, `0824E818`–`0824E8A8` | `data/actor_marker/*.csv` | 180 bytes: fragment frame/animation indices and facing-dependent XYZ offsets, with fixed group/fragment order and no pointers. |
| `resource_archive` | 10 ranges `08614DCC`–`08D5A85C` | `build/res/LABEL/index.txt` + `HASH.bin` (extracted) | The archives found by `ScriptProc_Start` (anm, map, ptn, fnt, sprites...). Header, sorted hash table and offsets are regenerated; entries are emitted as `.incbin` of their files and may change size. Their contents are position-independent: about 7,300 pointer-like words inside them (tile and map data) are no longer relocated. Verified in-game: growing the first `anm` entry by 37 bytes shifts every archive and leaves all menu and field screenshots identical. |
| `text_bank` | `08D6B1E4`–`08DD6B96` | `build/text/bank.tsv` (extracted) | Script text, resizable. The gcx section words, 9,977 entry offsets, then strings (`T`, decoded with `data/charmap.tsv`) and binary blobs (`B`, 82 tables read by script code). Verified in-game: a lengthened config help line renders and the field still plays. |

### English build

`make english` builds `build/english.gba` from the 2007 fan translation
([moozilla/boktai3trans](https://github.com/moozilla/boktai3trans), cloned
next to this repository). `tools/trans/english.py` imports the 7,212
translated strings of `scriptEd/script.sjs` into a copy of the text bank (the
rest stay Japanese; four use characters missing from the font), builds through
the normal pipeline, then applies the graphics, menu and code records of
`ips/latest_translation.ips` but not its text. That works because code and the
resource archives precede the text bank and do not move, and the patch's new
code sits in the ROM's zero padding, which the script checks is still free.
Checked in the emulator: the menu titles, config, sleep and save screens, map
names and the intro are in English; untranslated graphics (status-screen
labels, the `もどる` prompt) stay Japanese.

### Editing text

```
make            # extracts build/text/bank.tsv on first run
# edit a line:  1287<TAB>T<TAB><WEIGHT>Ａボタン</WEIGHT>で設定を変更します。
make mod        # build/mod.gba
```

Or use the local studio, which lists and searches all strings, previews edits
in the game font (with the game's wrap at the box edge and `<WEIGHT>` in red,
both checked against the emulator), saves to `build/text/bank.tsv` and runs
`make mod`:

```
python3 tools/studio/serve.py      # http://localhost:8642 (text), /gfx (graphics)
```

The graphics page browses the extracted archives: sprite packages (`CB05`)
with their animations, the 381 large sprites (`98F5`), the 195 palettes
(`92B3`: a 20-byte header and 256 colours), the font with its `charmap.tsv`
readings, and any resource as raw 4bpp tiles. The game picks a sprite's
palette at runtime (scripts and stage data), so colours come from the palette
chosen in the page.

Characters must exist in `data/charmap.tsv` (or use `{XXXX}` glyph codes);
ASCII is printed with the font's 8×16 half-width glyphs. Escapes are `\\`,
`\n`, `\t`, `\{` and `\xNN`. Markup such as `<PROC=0>` and `<WEIGHT>` is
plain ASCII and passes through unchanged. Restore the original with
`python3 tools/datafmt/check.py --extract text_bank`.

### Actor registry keys are string hashes

`sub_08219BD8` hashes a NUL-terminated string as
`h = ((h << 5) | (h >> 11)) & 0xFFFF; h = (h + c) & 0xFFFF` per byte, the
scheme of MGS's GCL `GV_StrCode` without its position term. Registry keys are
spread uniformly over 16 bits, consistent with being such hashes of the
original actor names, which the script compiler hashed offline (the names are
not in the ROM). With 722 keys, a random string matches some key about 1.1% of
the time, so a candidate name needs independent evidence: hashing the ROM's
own strings gives 167 such collisions, including the message speaker tag
`HAKUSYAKU` ("count"), whose key belongs to the engine callback at `08003A68`, not to an actor. Treat any
name recovered by hashing as a hypothesis until a second fact (the callback's
behaviour, a matching family of names) supports it.

### Actor script command tables

The actor member at +0x250 points to an ordered command-handler array.
`SceneActorScript_OnCommand` (`08136F1C`) reads a command packet's first
halfword, indexes that array, and invokes the selected handler with the actor
and packet pointer through `sub_08249240`. Handlers that only use the actor
ignore the second argument. This command index is separate from the hashed
registry key used by GCL `chara` and `command`.

`data/actor_commands/` contains the common six entries and the Bat, Bee,
Ghost, Lun, Crow, Spider, Vorn, Skeleton, Octopus, and Ivy extensions. The
shared constructors install the common table; species setup may replace it.
CSV `index` values must be consecutive in physical table order. The `callback` column contains the original Thumb
address, including its low bit; `name` is informational and does not affect
encoding. Zero is allowed for an empty slot, and remains a plain zero word.
The fixed manifest extent prevents an edit from resizing a table implicitly.

The encoder emits nonzero entries as pointers. The normal symbolizer resolves
them to function symbols, so moving code moves the handler pointers. To change
a handler, edit its callback value, then run the usual data check and ROM build.
Do not reorder rows to group similar handlers: the numeric index is the
dispatch operation.

Studio's Actors page has a **Command tables** view (`/actors#commands`). It
shows every decoded entry in dispatch order, with links to the handler's C
and assembly. An actor page lists tables directly referenced by its registry
callback or loader and identifies the function containing each reference.
Tables named as literals in the callback or loader; initialization may
install another table later. Studio's ROM view gives each table its own block
and marks exactly 336 bytes across the eleven table extents as decoded.

### Actor marker fragment tables

`data/actor_marker/` holds the two nine-fragment frame groups, the two
animation-selection sets, and two groups of nine four-halfword offset records.
`ActorMarker_InitBouncingParticles` reads the frame and animation tables;
`ActorMarker_LaunchFragments` reads the offsets. Facing group zero is selected
for quantized directions one/two, group one for zero/three. Animation set zero
is selected for marker kinds 3/5/7/9/13/15, set one for the other kinds.

CSV coordinates are signed halfwords; `unknown` preserves the fourth
halfword without assigning it a role. The codec validates exact ranges, all
18 rows, physical group/fragment order and numeric bounds. Animation indices
must select one of the two embedded animations. Existing `gUnk_` linker labels
are retained so source consumers do not need to change with a data edit.

These are numeric tables, including four frame bytes at `0824E6D8` that the
pointer scanner previously interpreted as a code address. Their reviewed
intervals are listed in `symbols/nonpointer_ranges.txt`; the encoder emits
only numeric byte/halfword items. This prevents those indices being relocated
when code or data moves.

Edit the CSVs and run `build/venv/bin/python tools/datafmt/check.py` to check
the original bytes, or `make mod` for an intentional edit. Studio's Actors page
lists them at `/actors#tables`, with signed coordinates and links to their
exact consumer functions. The ROM view marks only their 180 bytes decoded;
the intervening parameter tables remain separate, undecoded intervals.
