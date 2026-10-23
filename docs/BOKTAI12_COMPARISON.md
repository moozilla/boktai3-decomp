# Boktai 1 and 2 against Boktai 3: preliminary engine comparison

Tags: #research #comparison

A first pass, not a decompilation. Method: byte-pattern alignment of whole ROMs
(no symbols exist for 1 and 2), then B3's function names carried across by
matching code. Numbers below come from `tools/crossgame.py` (per B3 function: where
is it in another ROM) and `tools/romalign.py` (which byte ranges of ROM A exist
in ROM B). Both mask Thumb `BL` targets and literal-pool words, so they ignore
relinking, but anything whose instructions differ (new struct layout, different
register allocation, edited logic) counts as "not shared". Treat every percentage
as a **lower bound on similarity**.

ROMs used (all 16 MiB): B1 JP `U3IJ`, US `U3IE`, EU `U3IP` (five languages), US
Sample `A3IJ`; B2 JP rev 0 and rev 1 `U32J`, US `U32E`, EU `U32P`; B3 JP `U33J`.

## 1. Short answer

* **B3 is the B2 engine, substantially rewritten; B1 is a distant ancestor.**
  Of B3's Thumb code, about 15% is verbatim (modulo addresses) in B2 and about 3%
  in B1. By function: 2,501 of 9,452 B3 functions have a >=50% match in B2 JP
  (8.7% of code bytes at >=80%, 3.2% identical), 706 in B1. The gameplay layer
  (Player, enemies, scene actors, reactions) is almost entirely different: e.g.
  `Player_*` 3% shared, `Golem/Vanargand/Mummy/...` 0%.
* **The kernel is the same in all three** and sits at the end of the code
  region in the same order: task system, heap, resource registry/hash table, the
  script VM core, terrain, serial/RFU glue (B1 has the serial layer but no `rfu`), solar clock and
  moon/sun math, save/EEPROM, MP2K sound, libgcc/libc. Library tags: `EEPROM_V122`
  (B1) vs `EEPROM_V124` (B2, B3); `RFU_V1024` from B2 on; `SIIRTC_V001`, MP2K
  everywhere.
* **Shared B2 to B3 on top of the kernel:** dialogue and message-window code
  (`TextState_ProcessTag`, `DialogueTransition_*`), ScenePanel/HUD, `SceneObject*`,
  particle/effect pools, `ChoiceMenu`, wireless menus (mostly), the font.
* **Data:** the 16x16 kana/kanji font of B3 matches B2 JP (61% of it; B1 0.6%, so
  B1 used a different font). MP2K samples: 50% shared with B2, 33% with B1
  (reused instruments and sound effects). Sprites 24% / 6%, music tracks 23% / 6%.
  Text and event bytecode: about 0-1% (new content).

## 2. What is shared where

Largest runs of identical code, B3 range to B2 JP rev 1 (full list: rerun
`tools/romalign.py baserom.gba B2.gba`):

| B3 range | what | B2 JP address |
|---|---|---|
| `08003B40-08004BD6` | `ColorFade_FromScript` .. `Scene_SpawnDirectionalDust` | `08002988` |
| `08006CC6-08008A70` | `ChoiceMenu_*` .. `Trigger_MoveDropsByItemFromScript` | `08005B4A` |
| `08032754-08036E14` | `TextState_ProcessTag`, `DialogueTransition_*` (18 KB) | `08049198` |
| `0804F21E-0805907C` | ResourceStation, BreakableProp, HitSwitch, StageIcon, ProximityIcon (about 30 KB) | `0809F1A0` |
| `08121B98-081272DE` | particle/effect pools | `081E9DC4` |
| `08215096-0822B5C8` | Sprite, Heap, Terrain, Serial, Engine, Clock, Time, Sound (about 60 KB) | `0822B01E` |
| `0822ED64-08231438` | Save (EEPROM wrapper), Script core | `08243648` |
| `08243C0C-0824DAA8` | StatGauge, Sio32, Eeprom, libgcc, libm, memcpy | `08247534` |

Against B1 the same regions survive from `08215096` on (kernel: `ResourceRegistry`,
`Script_*` operand/variable handling, `HashTable`, `Serial`, `SceneLifecycle`, `SunClock`,
`Time_*`, `Eeprom`, soft-float) plus scattered effect/Actor-phase pieces. So B1 already
had the GCL-style keyword script VM, the sun clock and task system; B2 added RFU wireless,
B3 added a new actor/scene layer on top.

Per-subsystem share of B3 bytes also found in B2 (>=50% match): ScenePanel 43%,
Terrain 43%, `rfu` 97%, MP2K/libs about 97%, TetherActor 12%, Hud 10%, everything
gameplay below 6%. `wip/crossgame/b3_functions_in_b1_b2.csv` (untracked) lists, for
every B3 function, its address in each of the eight ROMs (`~` = approximate match).
That is a ready-made symbol table for 1 and 2 for anything in the kernel.

## 3. Script system is the same format

Text and bytecode use the same inline tag syntax in all of B1, B2, B3
(`<PROC=..>`, `<END>`, `<LABEL=..>`, `<NAME..>`, `<WAIT..>`, `<VAR..>`;
B1 additionally `WEIGHT`, `OTENKO` heavily, B2 `EXTEND`, `ALTER`). `ChoiceMenu_ParseTag`
is byte-identical in B2 JP/US/EU and B3. `Script_DecodeOperand`, `Script_ReadTypedVariable`,
`Script_WriteTypedVariable`, `Script_ApplyOperator` are the same VM core (B2 US variants differ slightly).
Consequence for the translation: B2's text tooling and the vocabulary of tags carry over;
B2's EU ROM is a worked example of multi-language packaging of exactly this format.

## 4. Text and font: what differs between JP, US and EU (Boktai 2)

Short version: **the text engine code is the same in all Boktai 2 regions; localization is
data** (font, escape table, strings, a few layout constants). Checked by comparing
the 2,448 functions that exist at a known address in JP rev 1, US and EU with
literal pools and `BL` targets masked (`tools/crossgame.py`, then a halfword compare):
only 46 differ JP to US and 80 JP to EU, and none is in the text path.
`Text_DrawGlyph`, `TextState_RenderNextGlyph`, `Text_DecodeGlyphEscape`,
`TextWindow_Draw*Glyph`, `Text_LookupString`, `MessageWindow_*`, `ChoiceMenu_ParseTag`
are identical. The ROM used for "JP rev 1" is `AGB-U32J-1`, SHA-1 `1a81843c...`, the
same ROM that [akatsuki105/boktai2](https://github.com/akatsuki105/boktai2) decompiles
(functions there are `FUN_08xxxxxx`, so addresses in
`wip/crossgame/b3_functions_in_b1_b2.csv`, column `b2_jp1`, apply to it directly).

### 4.1 Glyph encoding (same code in B2 JP/US/EU and B3)

`TextState_RenderNextGlyph` (B3 `08032A44`; B2 JP `08049488`, US `080493B0`, EU `080495D0`):

* byte `0x20..0x7F`: narrow glyph `byte - 0x20` (8x16, 4bpp, 64 B). Only 96 narrow glyphs
  are reachable this way.
* byte with bit 7 set: lead byte of a two-byte wide glyph (16x16, 128 B),
  `((b & 0x7F) << 8) | next`; ignored (cursor advances) if `>= Text_GetWideGlyphCount()`.
* byte `0x1F` + key: **escape**. `Text_DecodeGlyphEscape` looks the key up in a table of
  `{u16 key, u16 value}` (B3 `086055E4`; B2 JP `085AB45C`, US `085A629C`, EU `085ABB64`);
  `value < 0x100` selects narrow glyph `value` (this is how glyphs 96+ are reached: accented
  Latin), `0x80xx` selects wide glyph `xx`; no match draws the plain `0x1F` glyph.
  EU text uses it everywhere (`Ata\x1f\x1dd` is "Ataud" with u-acute; `DR\x1f\x07CKEN` is DRUCKEN with U-umlaut).
* `<TAG=..>` markup is parsed by `TextState_ProcessTag`; same tags as B3.
* Glyph widths are fixed (narrow 1 column, wide 2 columns); there is no width table.

The escape tables are identical in JP, US and EU except for two entries, `4F` and `50`
(JP/US: wide glyph `0x15`/`0x16`, EU: wide `0xFA`/`0xFB`). **B3's table is the EU one.**
akatsuki105's `charmap.txt` names wide `0xFA`/`0xFB` as the ligatures OE and oe.

### 4.2 Font resource (`fnt`, key `A635`/`3F51` in every ROM)

Layout in every ROM: `u16 narrow, u16 wide, u32 0x0C, u32 0x0C + 64*narrow`, then
narrow glyphs, then wide glyphs (`tools/b12text.py font ROM`).

| ROM | header | narrow | wide |
|---|---|---|---|
| B3 JP | `089F8EF4` | 128 | 1,537 |
| B2 JP | `089EE090` | 128 (identical to B3) | 1,537 (371 identical to B3) |
| B2 US | `089E40D4` | 128 (identical) | 257 |
| B2 EU | `089E26DC` | **156** (first 128 identical) | 257 (same as US) |
| B1 JP / US / EU | `08B97548` / `08BA54E4` / `08B6CE70` | 191 / 262 / 151 | 1,281 / 257 / 257 |

* Narrow glyphs 96..127 are accented Latin in **all** of them (the same shapes).
  EU adds narrow 128..155: glyphs 128..149 are the rest of the accented set (lowercase
  u/y with accents, n-tilde, sharp s, guillemets, inverted ? and !, a-ring, degree sign)
  plus a few symbols; 150..155 are symbol glyphs. These are reachable only through the
  escape table, whose keys already point at 0x80..0x95 in the JP, US and B3 ROMs.
* **B3 therefore has the escape table for the full EU Latin set but not the glyphs.**
  `TextWindow_DrawNarrowGlyph` computes `data + glyph*64` with no count check, so
  an escape to glyph 128+ reads the beginning of the wide font as pixels. Adding the
  28 glyphs means growing the narrow block (`narrow=156`, `wide_off += 28*64`) and
  re-pointing the one-entry archive that holds `fnt` (B3 `089F8ED8`, directory `08614D6C`);
  there is 1.47 MiB free at the end of the ROM.
* US/EU wide font = the first 255 JP wide glyphs plus the localized `0xFA`/`0xFB` (JP has
  blocks there, US/EU have OE/oe) and glyph 256. B3 reordered its wide font (only 371 of
  its first 1,537 equal B2 JP's).
* A sheet of the EU narrow glyphs 96..155 is easy to regenerate from the ROM
  (`tools/b12text.py`; the `fontsheet_eu.png` made for this note is derived from ROM
  data and is not committed).

### 4.3 Script image and text banks

All B2 ROMs have one `gcx` image with the B3 layout (`{timestamp, proc table, 0xFFFFFFFF,
4 section offsets}`; `tools/b12text.py gcx ROM`):

| ROM | proc table ends | string table | string data | strings | body |
|---|---|---|---|---|---|
| B2 JP | `08CCA69C` | `08CCA6AC` | `08CD1640` | 7,141 | `08D13424` |
| B2 US | `08C99998` | `08C999A8` | `08CA0A80` | 7,222 | `08CE4B98` |
| B2 EU | `08CB1BB0` | `08CB1BC0` | `08CD3A58` | 34,726 | `08E2F0E8` |
| B3 JP | (see ENGINE.md) | `08D6B1F8` | `08D74DDC` | 9,977 | |

* **EU: one table, five language blocks** in menu order: English, French, German,
  Italian, Spanish. Each block repeats the same ids in the same order; block bases
  (confirmed live, see below) are EN 0, FR 7080, DE 14143, IT 20897, ES 27641 (the item
  list `EARTHLY NUT` is id 1 of each block); block sizes 7,080 / 7,063 / 6,754 / 6,744 / 7,085.
  The four "press start" strings of the other languages sit just before each base
  (7077-7080 etc.), i.e. the title screen shows all of them.
* **The language is chosen by the script, not by the engine.** In EU bytecode a string
  operand appears five times, once per language, with ids offset by the block bases
  (observed live: `717, 7797, 14860, 21614, 28358`, which is 717 + the block bases);
  `Script_DecodeOperand` runs `Text_LookupString` on each and the command takes the one for
  the selected language (the language-select screen sets it; it is the first thing an EU
  ROM shows when there is no save). US and JP scripts carry one id per string.
* Item and place names stay English in all EU blocks (`EARTHLY NUT`), descriptions and
  dialogue are translated. Strings contain the usual `<PROC>`, `<LABEL>`, `<WEIGHT>`.
* Boktai 1 has a different text engine and was not decoded: string tables of about
  1.6k-1.8k entries sit at `08E298EC` (JP), `08E19DA4` (US) and `08DED04C` (EU, five banks
  again), the narrow font has 191/262/151 glyphs, and `Text_DrawGlyph` is not found in B1.

### 4.4 What else differs between regions

Code that is not in the text path but changes anyway (counted by masked halfwords; many are
single layout constants):

* **JP-only password code** (`GlyphTable_FindPair`, B2 JP `08011178`, called from
  `src/password.c`'s equivalent): a kana-pair lookup absent from US/EU, which shrink the
  code by 208 B there. B3 still has it (`08011578`).
* EU only: `SolarBank_BeginOpening` (a position constant `185 -> 233`),
  `TransitionPanel_UpdateCostSelection`, `SaveMenu_Update`, `ScriptSelector_UpdateTask`,
  `Presentation_ConfigureLayout` (143 halfwords), plus about 25 one-halfword edits in
  HUD/menu code: layout nudged for longer text, not logic.
* `SunClock_*`, `Time_CalculateSunriseSunsetCore`, `Clock_DaysInMonth` and `Script_ApplyOperator`
  differ JP to US/EU in a handful of halfwords; JP rev 0 to rev 1 also touches SunClock.
  Not examined further.
* A few tens of KB of B2-only code (around `08115000` and `081B8000`, enemy/boss
  behavior) is not byte-aligned between JP and US; it may simply be a revision difference
  and was not examined.

## 5. What this means for the B3 translation

1. **The `0x1F` escape convention and its table: nothing to invent.** B3 already carries
   Konami's own Latin escape table; B2 US/EU text is a corpus of how Konami used it.
2. **The missing piece is 28 narrow glyphs** (128..155) and the font resource relocation
   described in 4.2. Everything else for French/German/Italian/Spanish accents exists.
3. B2 EU's per-language operand scheme is the model if a multi-language B3 is ever wanted;
   for a single-language patch, replacing strings in place (or adding a text bank and
   moving `Text_LookupString`'s table pointers) is enough.
4. Widths are fixed 8 px: no proportional font exists in B2 US/EU either, so line
   lengths in the translation are bounded by the window width in columns, and
   `<WEIGHT>`/`TextState_MeasureStringLine` already measure in columns.
5. akatsuki105/boktai2 (JP v1.1, matching decomp, `charmap.txt`, `tools/encoding/`) is the
   closest external reference for B2; its names are `FUN_<addr>`, so the CSV here is
   the bridge to our B3 names.

## 6. akatsuki105/boktai2 compared with this decomp

Cloned `akatsuki105/boktai2` (main, last commit 2026-10-22, 41 commits since 2026-10-11)
and `akatsuki105/agbcc` and looked at both.

**Progress (from sources, not a build; no deno/agbcc on this machine to run his `make`):**

* Target: Boktai 2 JP rev 1 only (`AGB-U32J-1`). Function states, per his CLAUDE.md:
  TODO (asm blob only), NAKED (`INCFUNC`), NON_MATCH (`NONMATCHING_C`), MATCHING.
* Still asm: 2,929 `INCFUNC` functions (2,374 NAKED + 555 NON_MATCH; 1.72 MB of
  assembled text) plus 3,636 functions inside 88 `asm/*.inc` blobs that are `INCASM`'d
  whole (about 1.2 MB). By count that is about 6,500 functions still asm. His tree
  has roughly 6,300 C function bodies (regex count; includes small statics and the
  NON_MATCH drafts), so the order of magnitude is **a third to a half done by
  function count**, but the unmatched half is the bigger code (ax/boss/enemy blobs and
  large functions such as `FUN_080e9178`, 70 KB of source), so **by bytes it is
  probably well under half**. These sums double-count (blobs and INCFUNC assembled
  size exceed the 2.4 MB code region), so treat all of it as rough.
* He is not at "engine matched, gameplay left"; it is the reverse: low-level and menu
  code is largely C, the enemy/boss/entity code is still asm blobs.
* Names: his are by subsystem (`Entity329B_*`, `SpriteHolder_*`, `LinkBattleLobby_*`, plus
  `FUN_<addr>` for 2,349 still unnamed). Joining his `.inc` addresses with the crossgame
  CSV gives 293 functions in both trees; for the 62 with real names they agree with ours
  (`Entity329B_*` = our `StatusScreen_*`, `Entity0805fe30_*` = `DamageNumberDisplay_*`,
  `Entity1E98_Create` = `HitSwitch_AddFromScript`, `SpriteHolder_SetAnim` =
  `DisplayAnimation_SetEntry`, `EntityD9AE_*` = `SceneObjects_*`). That is independent
  confirmation of the cross-ROM matches. Most of his named functions are already C, which
  has no address in the repo, so the join cannot see them without building it.
* His tooling is a near twin of ours (m2c, decomp-permuter, ghidra split, Claude-driven
  workflow, `charmap.txt`/`tools/encoding` for data). His `docs/for-ai-agent/agbcc-internals.md`
  (decision rules read from agbcc's source: register class, switch lowering, `tst` never
  emitted, push mask) and `agbcc-levers.md` (ordered tiers of things to try on a near-miss)
  overlap with our `docs/REGALLOC.md` and the agbcc idioms notes and are worth
  a read; his policy is stricter than ours (no `register asm("rN")` pinning; we allow it
  as a marked concession).

### 6.1 `thumb_patch03-OCT-03`: is it worth it for Boktai 3?

What it is: Nintendo's official patch to the AGB SDK's `cc1` (Thumb), revision of
3 Oct 2003, with an earlier 30 May 2003 change. akatsuki disassembled the shipped `cc1.exe`
and added two flags to pret's agbcc (`akatsuki105/agbcc`, one commit on pret's `da598c1`,
which is the commit our `tools/setup.sh` pins, so the diff is exactly that commit):

* `-fsigned-narrow-modes`: `PROMOTE_MODE` stops forcing `UNSIGNEDP = 1`, so `s8`/`s16` values
  keep their declared signedness in registers (e.g. `ldrsh` instead of `ldrh`).
* `-ffix-shift-compare`: a different rule when `simplify_comparison` folds a compare
  against a shifted value (also converts the condition to unsigned for `LSHIFTRT`).

**Tested here** (`tools/flagsurvey.py`: compile every unit, 9,602 `src/*.c`+`src/fn/*.c`, with
the pinned agbcc and with the fork plus each flag, compare assembly):

| flag set | matched units whose output changes | NONMATCHING units that change |
|---|---|---|
| fork, no flags | 0 (identical to pinned) | 0 |
| `-fsigned-narrow-modes` | **99 of 9,473** | 11 of 454 |
| `-ffix-shift-compare` | 2 | 0 |

Any matched unit that changes would stop matching, so **Boktai 3 was not built with the
patched narrow-mode behavior** (nor the shift fix in those 2 units). For the 11 NONMATCHING
units that change, `tools/check.py --nonmatching` with the flag made 9 of 11 worse
(for example `sub_08084BCC` 8 differing bytes to 243, `sub_081376EC` 8 to 804), left `player_3`
unchanged and nudged `sub_081C85F4` from 184 to 176 differing bytes (a 77% fuzzy draft, so not
informative); none became exact. The same conclusion akatsuki reached for Boktai 2
(2 of 283 objects change; his `SwapArmorSlot` wants `ldrh`, which is the unpatched result),
so both games appear to have been built with the unpatched compiler despite shipping after
the patch date.

Verdict: **not worth pursuing for the compiler.** What is worth taking from his repo is
the documentation of agbcc's decision rules and the Boktai 2 names and structure.
(A caveat: only flags that are encoded in the compiler are tested; if different units
were built with different SDK revisions the survey would show some units *improving*, and
none do.)

## 7. Not done / next steps

* Name B2 functions (and then B1) properly: import the CSV into a B2 symbol set and propagate
  names through callers.
* Examine why the two large JP/US code gaps exist (revision differences or intended).
* Decode B1's text engine (different font header, different glyph counts).
* Confirm which EU glyph is which key (the 28 new ones) with a rendered sheet when drafting the B3 font patch.

Tools: `tools/crossgame.py OTHER.gba [--shape] [--win N] [--out f.csv]`,
`tools/romalign.py A.gba B.gba [--csv f]`, `tools/b12text.py {font|gcx|escapes}`
(all need numpy except `b12text.py`), `tools/flagsurvey.py FORK_AGBCC OUT.json`.
