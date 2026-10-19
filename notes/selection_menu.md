# Selection menu unit (tentative)

Tags: #selection-menu #translation-unit #matching

Range: 08019E58–0801B49C (end exclusive). Consolidated the 45 input files
into `src/selection_menu.c`, with every function and remaining assembly body in
ROM order. `include/selection_menu.h` holds the shared owner, script-call
arguments, packed-copy position, and the observed actor/item table layouts.
The owner reuses `Task`, `SceneInputResource`, `SceneInputWidget`,
`SceneInputPanelDrawable`, and `ContactPosition` from existing headers.
The `0xDB4` allocation includes two resource headers, the panel drawable and
its graphics, two panel widgets, three cursors, eight markers, a highlight,
a heading, nineteen icon/choice widgets, a sixteen-byte result record, the
state callback, and three message sources. Semantic names remain tentative.

## Matching results

The original 45 C functions remain exact. Newly matched, without pins:

- 0801A3C0: `SelectionMenu_InitCursors` initializes three cursor widgets.
- 0801A418: `SelectionMenu_InitMarkers` initializes eight marker widgets.

These add 176 exact bytes, a 0.0073 percentage-point code gain.
Names use file-local aliases; no function-symbol CSV was edited.

0801ACB8 (`SelectionMenu_NavigateGrid`, tentative) is a guarded NONMATCHING
draft, recorded at 94.7% fuzzy by `tools/check.py --nonmatching --record`.
It selects a direction from newly pressed keys, delays held-key repeat, wraps
the four-by-four grid, and resets the repeat counter after movement. The
remaining differences involve the held-input pointer lifetime and the
compiler sharing branches/masks between the down and right wrap cases.
The default build retains the original assembly.

0801A218 remains assembly: panel widget/sprite setup, resource 922E/4BBF,
glyph 593A, palette 1F7, and a stack-local sequence step. A typed draft was
about 65–67% fuzzy; the sequence step's masks, instruction ordering and
register allocation still differ. 0801A6D0 remains assembly: initializes
nineteen icons in reverse order, then the heading at (32, 0). Attempts
reached about 70% fuzzy; scratch-position lifetime, final heading argument
ordering and register allocation differ. These lower-scoring attempts were
left in ignored build scratch, not production source.
No extra/gap functions were needed: the exact unit spans all 5700 bytes.

## Matching adaptations and API observations

- The two marker-position loops byte-index the `GameState` owner before
  accessing the shared `inventory[0]`/`party[0]` field. Direct array indexing
  reverses the two address additions. No alternate partial struct is used.
- Marker hiding traverses pointers to the shared widget's flag word with
  the observed 0x60 stride; traversing widget pointers changes load offsets.
- Grid population retains the original packed bitfield position copy through
  a small shared packed-copy type. The original z/padding components remain
  unspecified, as in the preexisting matches; no new such behavior was added.
- `SelectionMenu_Choose` keeps its existing pinned byte-offset coordinate
  stores: using direct position fields coalesces the address calculations.
  The adaptations have source comments. No new pins or barriers were added.
- `SelectionMenu_Initialize` keeps its two unused creation arguments: dropping
  them changes the creator's saved registers and call setup. Task callback
  casts retain the engine's public void-returning callback declaration while
  the menu update/destructor retain their observed zero return values.
- The page-refresh routines explicitly pass `Script_ParseStringRef`'s return
  to `Text_LookupString`, replacing their old missing-argument declaration;
  this matches exactly with the existing engine prototype.
- `tools/depin.py src/selection_menu.c --write` kept all 19 existing hints.
  Existing concession markers stay beside the affected functions.

## Validation

Used `merge_unit.py`'s item/conflict/merge implementation (no conflicts after
type unification). Its CLI removal step runs `git rm`; source removal and
output writing were done directly to respect the driver's no-git-metadata
constraint. No commits, shared documentation edits, generated-source edits,
or tool edits were made.

Approximately four to nine focused checks per unmatched function, several
whole-unit checks, individual checks of all existing functions during type
unification, and the 19-hint depinning pass. Formatting was restricted to
the unit and its header. The final fast unit check reports zero differing
bytes across 08019E58–0801B49C. Progress reports two additional matching
functions and no lost matching bytes. Full-build result is recorded below.

The first full build exposed a fresh-checkout setup issue: private `build/res`
assets had not been extracted, so data assembly could not open its incbins.
Generated the missing resource archives and text bank from the ROM using
`datafmt` in this checkout only. Retained both normal and indexed extraction
filenames required by the shared generated assembly. No shared `gen/` files
were regenerated; the resumed build used these private build inputs.

Final validation: `build/venv/bin/python tools/build.py` printed
`build/boktai3.gba: OK` (byte-identical ROM). Build log:
`build/selection_build_final.log`. The merged source/header also pass the
scoped clang-format check and `git diff --check`. No commit was made.
