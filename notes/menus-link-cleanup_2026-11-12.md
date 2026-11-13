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
