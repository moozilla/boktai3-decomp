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

## Deferred countdown controller: 0804D620–0804D778

A typed CountdownTask (frame/interval at 0018/001A), existing Player and
GameState types match Tick, Destroy and Create. Initialize differs: the typed
interval/frame stores move the constant-zero return setup before the interval
store, requiring a copy of Script_GetValue's r0 result to r1 and four extra
bytes. Direct ternary assignment and one/three parameter forms preserve that
difference. The factory's initializer return contract must be signed to retain
its negative failure check. Do not retain a function-pointer cast as a workaround.
Clean typed attempt and header are in build/countdown_controller_attempt; all
four original files remain unchanged. Next attempt should examine the store/
return register lifetime with scope before changing the C shape again.

## Shaking icon: 08056D70–08056F20

Five functions share `src/shake_icon_display.c`, a real Task prefix and the
existing resource/animation layout. Published the creator declaration. A direct
symbolic random-table index changes the order of the address load and index
shift, so its existing fixed-table expression remains. Exact MATCH: 432 bytes;
only this unit includes the modified header.

## Countdown timer: 0805751C–08057678

Eight functions now share `src/countdown_timer.c` and a complete timer API with
Task, remaining time, lock and completion script. The manual shifted-integer
zero test became a direct check of the remaining halfword; the script-add
conditions became a single guard. All 348 bytes match; no other unit includes
the changed header.

- Deferred inventory transactions (08061C3C–08063634): all 41 original units pass baseline. The range mixes ControlView, Tail, ScriptCatalog, LifecycleView, Lifecycle, Panel, and local swap/sale/cleanup views of one 0x1D24 controller. Their catalog at 0x1AB3 overlaps transition storage at 0x1AF0 and the grid at 0x1BE4; reconciling function signatures requires changing upstream inventory-control/action APIs outside this range. A concatenation would preserve precisely the partial views this cleanup must remove. Preserve originals pending a coordinated whole-controller type migration.

HUD tilemaps: 11 functions consolidated. Four-digit display uses the canonical SceneInputPanel resource, label and smallDigits fields; numeric balance uses GameState.menuRewardBalance. Feature palette API named. The existing nonempty do/while(0) store grouping is retained because removing it changes register allocation. All 129 header consumers match.

Message panel task: consolidated the contained 20-function object at 081094EC–08109AEC. Shared real Task/resource/palette/state/script/window/timer/result fields replace three incompatible local object views and all raw task offsets. The state callback now uses a direct C call and matches. Keep the 12 preceding link-session APIs and four following snapshot helpers separate: they operate on the shared AreaEffectLinkState, whose phase-dependent sessionData overlaps snapshot, player-kind and exchange fields; replacing those views requires a coordinated session-family layout pass.

- Deferred Registry 1745 session lifecycle (0810B6DC–0810BB0C): the constructor and runner define divergent views of the existing AreaEffectLinkState singleton, including a 0x180-byte settings/snapshot workspace at 0xE0 and separate callbacks at 0xB0/B4. Its settings fields overlap player-kind and exchange payload fields already exposed by the shared header. Reconciling that phase union would touch active link/depinning consumers; preserve the five originals for a coordinated session-layout pass.

- Deferred link panel (0810C238–0810C724): typed widget/resource fields match the first three functions. The old peer-entry view places two 0x5FC records at 0x940, extending to 0x1538, but callback/control storage starts at 0xF40 and the creator allocates only 0xF78. A straightforward complete struct therefore moves role/status fields by 0x600 and cannot be correct. This is a sparse or phase-overlapping workspace, not a normal two-element record array. Preserve the five originals; attempted typed source/header retained in build/link_panel_attempt. The following slot controller uses the same disputed workspace and remains separate (also has an old_agbcc reset at 0810CB7C).

Link record panel 0810CBC8–0810D548: 19 files / 22 functions unified across the artificial subsystem boundary. Real Task, seven widgets/resources, palette cycle, two 0x850 previews using shared CharacterPreviewModel, text windows, scripts and timers. Incoming 0x24 style records and 0x40 time records now have field names. Direct state invocation matches. Anchored pointers step by sizeof their owning records to preserve model/flag loop code. Structured validity predicates changed branch polarity; retain that existing small goto helper. The old_agbcc reset at 0810D548 and following close wrapper remain separate.

CharacterPreviewActor is shared between link record and selection panels. It embeds the existing CharacterPreviewModel; extraction preserves the record panel MATCH.

Selection panel 0810DA38–0810E91C: 24 files unified with a complete 0xD38 layout. Reuses CharacterPreviewActor and real resource/widget types. Proper two-argument ResourceHeader_Relocate calls match the old underspecified declarations. The protocol writes only the low byte of canonical exchangeValue; keep an explicit byte lvalue. Preview setup keeps its model pointer based on the named preview actor, preserving the original allocation. Direct callback invocation matches. The old_agbcc reset at 0810E91C and final close wrapper remain separate.

- Deferred record menu (0810EDC0–0810F868): all 18 source files pass baseline. Its canonical record_menu.h is also consumed by active depinning file 0810E9D4 and by the next RecordList object. The current public API exposes resources as byte arrays and widget coordinates as direct x/y fields; replacing them with the canonical resource/widget/Task types requires changing that other worker's source and conflicting local global declarations. Preserve these originals; shared preview model extraction below prepares a later coordinated type migration.
