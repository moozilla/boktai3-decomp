# Menus, link and hardware unit cleanup

Tags: #cleanup #menus #wireless #types

## Result tutorial: 0801D104–0801E000

Consolidated 30 single-function files into `src/result_tutorial.c`. The shared
`ResultMenu` task now exposes tutorial phases, timers, fixed-point player and target
positions, input state, prompt state, resources, fifteen existing `SceneInputWidget`
objects, and four marker sprites. Unknown regions remain explicitly unknown.
All functions already carried descriptive ResultTutorial names.

Every original function passed its baseline check; the merged unit matches all
3,836 bytes. Existing blink gotos remain: a direct interval expression compiles
to a range subtraction, and nested ifs redirect the early branch through the
boolean comparison. Both differ from the original. Named widget pointers preserve
the base-plus-flags addressing. A named difficulty pointer also preserves the
initializer store order without its former unused-local scaffold.

No concession added. Exact-code coverage gain: 0.0 percentage points; this is
source organization and type recovery. The coordinator runs the combined ROM build.

## Message window: 080308BC–0803198C

Consolidated 41 files into `src/message_window.c`. One shared MessageWindow task
uses the full text playback state, shared SceneInputWidget, ContactSwarmSprite
and DisplaySequenceState layouts, a bounded message-frame table, and typed
palette banks. Seven unnamed functions gained provisional MessageWindow names.
The sole external live callback reference in MessageWindow_Create was updated
and that unit also matches. Two duplicate callback setters became one helper.

All original functions and the final 4,304-byte unit match. Canonical fields
remove raw offsets and separate the interpreter frame count from its pc. Former
custom/callback fields at 018C/0190 are page-advance delay/timer values. Existing
ram.h still declares the singleton with the generic Actor tag; the unit uses a
pointer conversion at that declaration boundary rather than changing this widely
included shared header during parallel work.

Shared API follow-up: engine.h declares TextState_SetBounds void, but the ROM
caller MessageWindow_SetBounds forwards its r0 result. A documented local
function-pointer signature preserves that call contract; no engine.h edit was
made. TextWindow_BlankRect's unused result assignment was removed. A signed
character-delay helper preserves the original signed halfword load. No new
concessions; exact-code coverage gain remains 0.0 percentage points.

## Scrolling dialogue: 08032EDC–080335D4

Consolidated 20 files into `src/scrolling_dialogue.c`, reusing the full message
playback state and exposing the scrolling task's requests, counters, row/column
and script fields. Four script wrappers gained ScrollingDialogue names. Five
simple fail/done goto wrappers became ordinary if/else and still match. A while
loop for the tag parser changed literal-pool placement and branches, so the
existing main playback flow remains. The final unit matches all 1,784 bytes.

The queue row ends at 080335F4, but 080335D4 is an old_agbcc singleton reset,
followed by a normal-compiler query. They remain separate rather than introducing
a new compiler override or compiling either with the wrong compiler. Neither
holds object fields to unify. Generic ram.h Window declarations are converted
at the task API boundary; shared headers remain unchanged.

## Scripted dialogue transitions: 08033C58–08034084

Consolidated twelve functions into `src/dialogue_transition.c`. The shared
DialogueTransition task exposes state-entry flags, the current and final message
indices, display control restoration, completion/skip procedures, and parallel
64-entry timing/bounds arrays. Initialize gained a provisional descriptive name.
Script keyword constants now read as characters; script failure gotos became
ordinary early returns. ConsumeStateEntry returns u8 consistently with its callers,
which retain the original return-width check. Exact MATCH: 1,068 bytes.

## Wireless text: 08036CA4–08036E08

Consolidated six functions into `src/wireless_text.c`. The API consistently uses
u16 character codes and u8 encoded indices. GetCharacterCode replaces the misleading
GetCharacterTableByte name. Named progress fields replace the initializer's two
anonymous counters; its indexed table address keeps the original byte-address
calculation. Fixed character-table storage remains at its existing cartridge RAM
address pending centralized ownership of that region. All 356 bytes match.

## Wireless status menu: 08036E08–08037228

Consolidated twelve functions into `src/wireless_status_menu.c`. A shared task
layout uses the real 0x60-byte widget and 32-byte resource header. The exit
procedure is an integer script key rather than a generic resource pointer.
Primary-background loading gained a descriptive name. Replaced the create and
script-loader goto branches with ordinary guards and returns. Shared palette
resources now live in `include/palette_resource.h`; all three previously landed
dependents passed their unit checks. This unit matches all 1,056 bytes.

## Deferred peer panel: 08037228–08037B20

The recovered menu has two 0x25C-byte panels beginning at 002C, window IDs at
04E4, a completion key at 04F0, scratch at 04F4, resources at 05F4/0614, and
an icon widget at 0634 (task size 0694). Each panel has a sprite at 0008,
graphics at 0034, five 0x60-byte digit widgets at 0050, name at 0230,
and a 24-byte confirmation-name buffer at 0244. The former changed byte at
0244 is the first byte of that name buffer, not an independent boolean.

Original baseline checks all pass. The first eleven typed functions through
08037730 match. The remaining problem is 08037788 and 08037960: their old
shifted Row view addresses a widget flag at panel+0058 and advances by 0060.
Real widget pointers instead anchor at panel+0050 and load/store at +0008;
indexed real-array loops anchor at the flags themselves (+0058). This changes
register allocation, stack size and instruction selection. Keep the real
panel layout, do not recreate the overlapping Row padding view. Typed attempts
and original sources remain under build/peer_panel_attempt and
build/peer_panel_original for the next pass. No source changes landed for this
row; continue in ROM order per the cleanup brief's resistant-type rule.

## Wireless exit task: 080394D4–080395D4

Four functions now share `src/wireless_exit.c` and a WirelessExit task containing
a completion script key and shutdown state. The old generic Task and listener
pointer views are gone. Creation uses the shared engine task API; all 256 bytes
match. Existing function names retained.

## Deferred wireless scene objects: 0803BAA0–0803CDF0

This row uses the same ScenePoolActor already defined by include/scene_pool.h,
not a separate wireless-object allocation. That header is consumed by 43 units,
including several units on the concurrent depinning list. A canonical merge
requires nesting the shared 44-byte scaled-parameter aggregate at actor+0084
and player+009C, while preserving the existing direct min/max-speed fields used
by the other family. It also needs shared model and spatial-index records at
0600, 052C/056C/0860. A new local WirelessSceneActor would perpetuate the partial
views explicitly forbidden by this assignment, and changing the shared actor's
field API crosses the small-header-change limit. No files in this row changed.
Coordinate a shared scene-pool type pass before consolidating it.

## Floating numbers: 08046B34–0804732C

Consolidated sixteen functions into `src/damage_number_display.c` and promoted
the existing shared display/group/glyph API into its header. Replaced the opaque
task prefix with Task. FindSource and Show now use structured return paths; the
allocation helper's shared exit remains because splitting it changes branches.
A shared projection wrapper also changed allocation in SetGroup, so the existing
relative/global projection forms remain. The 2,040-byte unit and all three
external header consumers (081A5360, 0820EE34, 0820F378) match. The old_agbcc
resets at either edge remain separate compiler units.

## Wireless link indicator: 080483A0–080485FC

Nine functions share `src/wireless_link_indicator.c` and the existing resource/
projected-display types. Replaced the opaque task prefix and promoted the
child-status alias to symbols/functions.csv. Creation now uses ordinary guard
returns and sizeof the real object. Exact MATCH: 604 bytes; this is the only
unit including its modified header.
