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
