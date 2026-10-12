# Handoff: state of the project and how to continue

Last updated after the scanline and status-screen matching batch (2026-10-12). This file is the single
entry point for a new agent, a new thread after compaction, or a helper on
another plan. Read it fully, then `docs/WORKER.md` (the matching playbook).

The [October 8 devlog](DEVLOG_2026-10-08.md) summarizes the session's technical
results. The [retrospective](RETROSPECTIVE_2026-10-08.md) audits progress versus
allowance and local per-response token records. It corrects the pilot's
reasoning-setting claim and identifies the Astra main thread as the dominant
published-rate cost proxy; exact subscription-cost attribution remains unknown.

## October 12 final lifecycle and adjacent message batch

Coverage is **18.546%**, up **0.014 percentage points** from 18.532%:
**3 exact functions / 328 bytes**, 5,336 total / 11,106.
Controller destruction and the adjacent scene-message enqueue/buffer swap match.
Previous PR #45 merged; reporting Action 38305054196 succeeded.
Combined validation: build/scene-lifecycle-combined.log.
Weekly allowance reached 50% remaining: preserve the user's 50% floor and
resume toward 20% when more allowance is authorized/available.
The controller resource initializer draft is in wip/sub_0802048C.c, restored
to its best initial form (432 differing bytes, 1176 vs 1168 target bytes).
Its first allocation block is almost exact; resource-loop register assignment
and increment order remain. Broad pinning worsened it; do not repeat blindly.
The adjacent dispatch helper 20A60 is also drafted but not exact. Keep drafts
out of production. Main object update F93C and three object initializers remain.

## October 12 scene-object commands and motion completed

Coverage is **18.532%**, up **0.068 percentage points** from 18.465%:
**14 new exact functions / 1,632 bytes**, 5,333 total / 11,106.
The command/query span 0801E000–0801EC4C is now completely matched. This includes
raise/lower/promote scripts, lock and both compatibility/resource predicates.
Both pose animations EF60/EFDC and the full raising/lowering callback chain
F3A0–F5EC now match, including impact effects and delayed script execution.
The controller remains incomplete: EC4C/F058/F5EC object initializers,
F93C main update, 203D4 destruction and 2048C resource setup remain assembly.
Combined validation: build/scene-motion-combined.log.
Previous PR #44 merged; reporting Action 38304277922 succeeded.
Usage: 51% weekly remaining; stop below 50%.
Matching notes: E700's R7 save issue is solved with an input-only pinned
constant and a literal OR in the body; allowing the compiler to see the
constant lets its body allocate/save R7. Return-result variables solved the
compatibility predicate's block order; explicit success-label placement solved
EC00's early return block. No Ghidra export exists locally.

## October 12 scene-object controller commands

Coverage is **18.465%**, up **0.097 percentage points** from 18.367%:
**17 new exact functions / 2,352 bytes**, 5,319 total / 11,106.
Object pose selection, slot construction, ID-list queries, script construction,
activation/deactivation commands, active/inactive state changes, unlock,
position readback and selection helpers now match. The contiguous controller
0801E000–08020964 has its own report group. It is NOT complete: pose eligibility,
lock, raise/lower/promote scripts, object callbacks and the main update remain.
The two independent selection helpers at EBBC/EBE4 are split out of EB08's
assembly tail. Nonmatching drafts remain ignored in wip; E700 previously had
only a prologue mismatch (fixed R7 omitted from save set).
Combined validation: build/scene-object-combined.log.
Previous PR #43 merged; reporting Action 38302938887 succeeded.
Usage: 53% weekly remaining; stop below 50%. Continue within this controller.

## October 12 result-game states and complete tutorial

Coverage is **18.367%**, up **0.221 percentage points** from 18.147%:
**33 new exact functions / 5,332 bytes**, 5,302 total / 11,104.
The complete contiguous tutorial 0801D104–0801E000 now matches. Its report group
names the prompt fades, marker motion, tracking and early/late/accurate press
examples, display blinks, script setup and table-driven sequence controller.
The preceding result game now has exact player motion, tracking and timed-press
state machines, graphics creation, resource initialization and script parsing.
Remaining result-game gaps: B600/B700 transformation calculation, BD44 graphics
layout and C24C graphics update. Readable drafts are retained in ignored wip.
C24C first draft build/c24c-first.c; later register experiments did not improve
it. A3C0 still has its four-byte prologue mismatch; do not repeat clobber tests.
Combined validation: build/result-game-states-combined.log.
Previous PR #42 merged; reporting Action 38301541113 succeeded.
Usage: 56% weekly remaining at last check; stop below 50%.
Continue contiguously at 0801E000; no new tooling or agents.

## October 12 transformation and result game

Coverage is **18.147%**, up **0.102 percentage points** from 18.044%:
**18 new exact functions / 2,468 bytes**, 5,269 total / 11,104.
Charge/reveal/finish animation, result reporting, marker animation, countdown,
command packets, both scoring paths, button handling and dispatch now match.
The new contiguous report group spans 0801B49C–0801D104 and is NOT complete.
Remaining: preview B600, apply B700, graphics BCA8/BD44/C24C, player motion
C6CC, state machines C874/CC24, and initialization CF6C/D020.
C6CC has a 19-byte register-only draft in build/c6cc-19.c; BCA8 is 30 bytes
away in wip. Other readable drafts are retained in ignored wip.
Combined validation: build/transformation-result-combined.log.
Previous PR #41 merged; reporting Action 38300138637 succeeded.
Usage: 59% weekly remaining at last check; stop below 50%.

## October 12 selection menu and presentation

Coverage is **18.044%**, up **0.157 percentage points** from 17.888%:
**25 new exact functions / 3,784 bytes**, 5,251 total / 11,104.
Actor/item graphics, availability, population, cursor placement, statistics,
selection/back/confirmation states, close/hide and script initialization match.
The selection-menu group is NOT complete: 1A218, 1A3C0, 1A418, 1A6D0 and
1ACB8 remain. Ignored draft 1A218 is 88 bytes away (build/a218-88.c); 1A3C0
has a body-exact four-byte prologue mismatch in wip; 1A418 sibling is similar.
Grid navigation is in build/acb8-natural.c. Avoid repeating blind pinning.
Combined validation: build/selection-presentation-combined.log.
Continue at 0801B49C, whose layer configuration is a sibling of exact 1A10C.
Previous PR #40 merged; reporting Action 38298881150 succeeded.
Usage: 64% weekly remaining at last check; stop below 50%.

## October 12 tile animation and sprite pool

Coverage is **17.888%**, up **0.067 percentage points** from 17.821%:
**15 new exact functions / 1,612 bytes**, 5,226 total / 11,102.
Palette frame cycling, eight-channel tile animation and the sixteen-slot sprite
pool are complete contiguous units, including two recovered byte-tail functions.
Animation playback 19C9C uses marked register constraints; the state logic is C.
Combined validation: build/tile-sprite-combined.log. Continue at 08019E58 into
presentation commands; most wrappers already match, so check inventory first.
Previous PR #39 is merged; main reporting Action 38298126393 succeeded.
Latest usage: 67% weekly remaining; stop below 50%.

## October 12 orbiting particles, circular windows and clock events

Coverage is **17.821%**, up **0.112 percentage points** from 17.709%:
**13 new exact functions / 2,712 bytes**, 5,211 total / 11,100.
Orbiting particles, circular scanline windows and clock events are now complete
contiguous units. Hardware blend update also matches. Five report groups name
these adjacent areas; palette cycling and blend initialization remain incomplete.
Ignored drafts: wip/sub_08018454.c, sub_08018664.c and sub_08018E00.c.
Continue contiguously at 08019518. Projection helper reuse matched 19130 on the
first check; keeping the RNG table register live solved orbital initialization.
Combined validation log: build/orbit-window-clock-combined.log.
Usage: 68% weekly remaining; stop below 50%. Previous PR #38 is merged and its
main progress Action succeeded (38296521524).

## October 12 scanline wave and weather states

Coverage is **17.709%**, up **0.078 percentage points** from 17.631%:
**12 new exact functions / 1,876 bytes**, 5,198 total / 11,100.
The scanline-wave unit is complete. Weather particle presentation, storm wait,
onset, active state, shutdown, dispatch and start/stop/cooldown commands match.
The weather family is NOT complete: scroll update 17510, particle initializer
17684, emission 177AC, trigger wait 17A6C and script initializer 18028 remain.
Best ignored drafts: build/17510-48.c (48 operand bytes); build/17684-five.c
(body exact, five prologue bytes differ; prologue-bugfix did not help);
build/18028-111.c (111 bytes). 17A6C has a readable wip draft.
Continue at the adjacent palette controller 18454; avoid prolonged allocation
searches. Combined validation: build/wave-weather-combined.log.
Usage check: 74% weekly remaining; stop below 50%. Prior batch merged PR #37
and its main-only reporting Action succeeded (38295175067).

## October 12 fading particles, palette levels and floating effects

Coverage is **17.631%**, up **0.083 percentage points** from 17.548%:
**13 new exact functions / 2,004 bytes**, 5,186 total / 11,099.
Fading-particle group movement/lifetimes and manager initialization, palette
level transitions/configuration/reset, and floating-item projection, bobbing,
waiting, departures and manager dispatch now match. Three report groups name
these adjacent families. They remain incomplete: constructors 15F60/17044,
palette script 16764, collection animation 16C04 and initializer 16F24 are drafts.
16C04 is 42 bytes away in build/16c04-42.c; 17044 has a 544-byte draft in wip/.
Do not count these as matches. Continue past 17270 if allocation work stalls.
Combined validation log: build/fading-particles-combined.log.
Latest usage observation: 75% weekly remains; stop below 50% as requested.

## October 12 particle-family continuation

Coverage is **17.548%**, up **0.150 percentage points** from 17.398%:
**15 new exact functions / 3,624 bytes**, 5,173 total / 11,098.
Particle-manager dispatch and initialization, fragment ballistic updates and
initialization, and animated-particle movement/playback, initialization,
configuration-based creation and script/wrapper emission are now exact.
Three named report groups cover these adjacent families. Recovered byte-tail
boundaries prevent crediting unmatched hidden functions. Volume emission 15A0C
remains six operand bytes away (ignored wip/sub_08015A0C.c); paused particle
rendering 14330 remains 20 bytes away (build/14330-20.c). Larger drafts 1404C,
14634 and earlier barrier destruction 1310C remain excluded.
Combined validation: build/particle-family-combined.log. Continue contiguously
at 08015F60. Latest usage observation: 81% weekly remaining; stop below 50%.

## October 12 hazards, barriers, and shadows continuation

Coverage is **17.398%**, up **0.148 percentage points** from 17.250%:
**16 new exact functions / 3,568 bytes**, 5,158 total / 11,094.
Animated-hazard constructors now match. Barrier initialization, updates and
placement, terrain/scaled shadows, the shadow registry and the first particle
update are matched and provisionally named. Barrier destruction 1310C remains
four operand bytes away in ignored build/1310c-four.c. Debris creation 12B34,
bouncing particles 1404C and paused particles 14330 remain ignored drafts.
The closest paused-particle candidate is build/14330-close.c (31 differing bytes).
Continue contiguously through the particle manager; do not stall on allocation
puzzles. This batch adds report groups for debris, barriers and shadow families.
Combined validation: build/hazard-shadow-combined.log. Usage: 82% weekly remains;
check periodically and stop below 50%, as requested. No reset credits consumed.

## October 12 password, scene-indicator, and animated-hazard batch

Coverage is **17.250%**, up **0.147 percentage points** from 17.103%:
**16 new exact functions / 3,560 bytes**, 5,142 total / 11,094.
The full scene-indicator span 11884–12040 now matches, including its large
creation script and terrain placement. Password filtering, checksum/version
validation, and decoded-data application are matched. Animated-hazard setup
and warning/attack/hold/retraction states now match; constructors 12748 and
12858 remain. Names are provisional. Register-pinned coverage is 2.246%.

Next: finish hazard constructors, then proceed contiguously beyond 12A88.
Ignored drafts: wip/sub_08012748.c; password wrapper build/11794-four.c is four
register bytes away. Password character validator 115DC remains a draft.
FAB8's animation C adapted directly to 12224/12380/124BC and matched; reuse it.
Usage guard requested by user: check periodically and stop below 50% remaining.
Latest observation was 91% weekly remaining; do not consume reset credits.

## October 12 status, group objects, and message-menu continuation

Coverage is **17.103%**, up **0.132 percentage points** from main's 16.971%:
**17 new exact functions / 3,188 bytes**, 5,126 total / 11,094.
The result prompt and message menu (111EC–1150C) are complete. The group-object
manager initialization/update/cleanup and several animation/command callbacks
are matched; its movement states remain partial. Status-screen initialization,
start-button wait, and fade/exit are now exact. Register-pinned coverage is 2.143%.

Next contiguous area begins at 0801150C. Prefer this text/menu code over another
long allocation search. Near candidates remain ignored under wip/ and build/:
E974 (8-byte clean candidate build/e9748.c), 10298 (8/11-byte variants), and larger
FC28/10514 movement drafts. Permuter score zero on E974 was NOT byte-exact: it
masked a branch-target difference. Always verify with the fast checker.

## October 12 scanline and status-screen batch

Coverage is **16.971%**, up **0.132 percentage points** from 16.839%:
**16 new exact functions / 3,200 bytes**, 5,109 total / 11,094.
Palette-set functions are complete. Their report span (0800E0CC–0800E3E0)
also retains the following unmatched reset tail at E3D4. Scanline presentation and
status-screen callbacks now have separate report groups and provisional names.
The latter covers numeric drawing, reveal/close transitions, palette blending,
callback dispatch, and cleanup. Register-pinned exact coverage is 2.069%.
EAF0 and E8E0 use a per-file optimization override; this is a matching
concession, not evidence of historical compiler flags.

Whole-session gain from 15.315%: **1.656 percentage points**. Continue toward
20% with adjacent callbacks. Remaining gaps include D874/DC90 scanline noise
and initialization, E5B8 numeric drawing, E974/EC0C/ECD0/EF90 transitions,
and F220 initialization. Drafts stay in ignored wip/ and are not counted.
F0F0 was an existing match rewritten with a descriptive name, not new progress.
Combined batch validation log: build/scanline-status-combined.log.
User requests periodic usage checks and stopping below 50% remaining.

## October 12 emitter and falling-object completion

Coverage is **16.839%**, up **0.186 percentage points** from 16.653%:
**17 new exact functions / 4,484 bytes**, 5,093 total / 11,094.
Effect emitters (0800C49C–0800C7F4), damage controllers (0800C7F4–0800CBA0),
and falling objects (0800CBA0–0800D874) are complete, with descriptive aliases
and separate report groups. The reset at CBA0 is a recovered function boundary;
it was already covered by the preceding allocator span, so adds no code bytes.
Combined validation: build/emitter-falling-combined.log printed ROM OK.
Whole-session gain from 15.315% is **1.524 percentage points**.
Continue directly with adjacent scanline/presentation controller 0800D874 onward.
Do not stop at each batch or add tooling; 20% remains the requested target.
Earlier gaps: item pickup 080070D0 and effect constructors 0800AC44/0800AFE8.

## October 12 effect updates

Coverage is **16.653%**, up **0.201 percentage points** from 16.452%:
**seven new exact functions / 4,848 bytes**, 5,076 total / 11,093.
The effect manager frame update and all six movement/animation callbacks now
match: orbiting, target contact, two homing variants, dissipating, and finished
animation release. A named report group covers 0800A800–0800C49C.
Names are provisional. Constructors 0800AC44 and 0800AFE8 remain unmatched;
do not mistake the completed update states for complete construction support.
Whole-session gain from 15.315% is **1.338 percentage points**; 20% remains open.

Current branch: codex/effect-manager-continuation.
Combined validation log: build/effect-updates-combined.log.
Continue actual matching without tooling changes. The adjacent effect-emitter
hit callback 0800C49C has an exact draft in wip/ (not yet production).
Its manager at 0800C520 and initialization at 0800C654 are next.
The item pickup callback 080070D0 also remains unfinished.

## October 12 directional and collision controller completion

Coverage is **16.452%**, up **0.178 percentage points** from 16.274%:
**22 new exact functions / 4,300 bytes**, 5,069 total / 11,093.
Directional interaction controllers (08009500–08009E30) and collision
controllers (08009E30–0800A800) are **100% matched**, with named report groups.
The former handles actor proximity/facing, button input, enter/exit scripts,
placement, and lifecycle. The latter handles hit/idle animations, timed effects,
scene pause transitions, creation, and cleanup. Names remain provisional.

The adjacent effect manager at 0800A800 is underway: its global cleanup,
particle callback dispatch, and initializer now match. The frame update has a
close draft in `build/effect222.c` / `wip/sub_0800A8E4.c`; the large constructor
at 0800AC44 is next. The item pickup callback 080070D0 also remains unfinished.
Neither draft is counted or stored in production source.

Whole-session gain from 15.315%: **1.137 percentage points**. Continue toward
20%; no tooling work or broad tests. Combined ROM validation printed
`build/boktai3.gba: OK` in `build/directional-collision-combined.log`.
Branch: `codex/directional-interaction-controllers`. Two independent global
cleanup boundaries (08009E30 and 0800A800) were separated without changing
the byte denominator. PR27 is merged; its main CI run 38266641331 succeeded.

## October 12 items and pooled interactables

Coverage is **16.274%**, up **0.331 percentage points** from 15.944%:
**28 new exact functions / 7,984 bytes**, 5,047 total / 11,091.
Pooled interactables (08008A70–08009500; tentative name) are **100% matched**.
This includes resource loading, pooled allocation, route/target/explicit
placement, state callbacks, activation effects, visibility, and cleanup.
Items and containers (08006F00–08008A70) are **91.97% matched**; only the
pickup-proximity/response callback at 080070D0 remains. Its best close draft
is `build/interaction159.c` (not exact; do not count or promote it).
The adjacent controller from 08009500 is underway in ignored `wip/`.

Whole-session gain from 15.315%: **0.959 percentage points**. The 20% target
remains open. No tooling changes or broad regression tests. Combined ROM
validation printed `build/boktai3.gba: OK` in
`build/items-interactables-combined.log`. Branch: `codex/actor-trigger-controller`.
Two named report groups and provisional function aliases accompany the matches.
The item/container field called `sound` at offset 0x16 is probably a region ID;
`sub_0822C034` takes a position and determines its map region. Interpret older
field names provisionally until the next focused naming cleanup.

## October 12 terrain modifiers and choice menu

Coverage is **15.944%**, up **0.133 percentage points** from 15.810%:
**11 new exact functions / 3,216 bytes**, 5,019 total / 11,091.
Terrain modifiers (08005E10–08006374; tentative role) and the choice menu
(08006374–08006F00) are now **100% matched**, with named report groups.
The menu supports row/column navigation, confirm/cancel dispatch, script
arguments, text layout and ALTER/EXTEND/VAR markup. The modifier manager
registers and changes terrain cells through script commands.

The menu constructor at 08006CD0 has its own verified boundary; the code-byte
denominator is unchanged. Whole-session gain from 15.315% is **0.629 percentage
points**. Continue into actor triggers at 08006F00; the 20% target remains open.
No tooling or broad regression tests. Combined build printed
`build/boktai3.gba: OK` in `build/terrain-menu-combined.log`.
Branch: `codex/terrain-modifiers-and-choice-menu`.

## October 12 scene-object completion batch

Coverage is **15.810%**, up **0.197 percentage points** from 15.613%:
**14 new exact functions / 4,760 bytes**, 5,008 total / 11,090.
The scene-object subsystem (080041CC–08005E10; tentative name) is now
**100% matched** and has a named report group. This completes the manager,
creation and palette scripts, pushing and step-down checks, movement/fall/
disappearance/flash callbacks, and directional dust/edge particles.
The independent cleanup at 08005E10 starts the next controller and now has
its own boundary. The byte denominator is unchanged. Continue from 08005E1C.

Whole-session gain from 15.315%: **0.495 percentage points**. The 20% goal
remains open. No tooling changes or broad regression tests. Branch:
`codex/scene-object-movement`; combined log: `build/scene-object-combined.log`.

## October 12 color, terrain-overlay and scene-object batch

Coverage is **15.613%**, up **0.123 percentage points** from 15.490%:
**14 new exact functions / 2,968 bytes**, 4,994 total / 11,089.
Color/fade controllers (080036F8–08003B44) and terrain overlays
(08003B44–080041CC; tentative name) are now **100% matched** and have named
report groups. The terrain-overlay update tracks actors, examines neighboring
map cells and reuses drawables for exposed terrain edges. Its complete frame
routine matches without register pinning.

The next contiguous scene-object area is underway: neighbor-cell eligibility,
movement effects, terrain interaction costs, activation/stop handlers and the
break-effect burst now match. These roles remain provisional. Remaining local
drafts include 080041CC (edge particles), 080046F0 (falling objects), 08004B64
(directional dust) and 08004CE4 (step down). Continue into their adjacent
movement callbacks; do not count these drafts as progress. The whole-session
gain from 15.315% is **0.298 percentage points**. The 20% goal remains open.

Branch: `codex/color-and-scene-controllers`. Combined validation log:
`build/color-terrain-scene-combined.log`. No tooling changes or regression tests.

## October 12 background and palette completion batch

Coverage is **15.490%**, up **0.176 percentage points** from 15.315%:
**16 new exact functions / 4,240 bytes**, 4,980 total / 11,089.
Both **Background controllers (080015AC–08002AA8)** and **Palette controllers
(08002AA8–080036F8)** are now **100% matched**. The new inventory boundary at
080020BC separates an independently matched map-loading wrapper; the code
byte denominator remains 2,415,354. The 20% target remains unfinished by
**4.510 percentage points**. Continue into the adjacent color/fade controller
starting at 080036F8; its command application and queue functions remain.

Work is solo, matching only. No new tooling or regression tests. Empty asm
constraints used with marked local register hints emit no instructions; they
prevent old agbcc from discarding the hint. Full build log for this batch:
`build/background-palette-combined.log` printed `build/boktai3.gba: OK`. Branch: `codex/background-completion`.
The earlier background WIP checkpoint is superseded by exact production C.

## October 12 subsystem reporting and background setup batch

Coverage is **15.315%**, up **0.116 percentage points** from 15.199%:
**10 new exact functions / 2,812 bytes**, 4,964 total. The 20% target remains
unfinished by **4.685 percentage points**. Continue with contiguous subsystems.

The report now has fifteen named contiguous game groups alongside the existing
libraries, including the already fully matched scheduler, script VM, registry,
message queue and solar sensor. Names and grouping do not change byte credit.
Boot/header/ARM code is distinguished from the already matched `AgbMain`.
See [reporting policy](PROGRESS_CI.md) and
[background controller notes](BACKGROUND_CONTROLLERS.md).

Matching additions cover background setup and script/queued commands in the
early-ROM controller pair, plus the neighboring scheduler's DMA copy utility.
The early background group is still only partly matched: both frame-update
routines and map-loading helpers remain unfinished. Source drafts are retained
separately; do not confuse descriptive report labels with exact C acceptance.
A two-minute input-polling permuter search did not produce an exact match.
Unfinished sources are preserved on `codex/wip-subsystems-2026-10-12` under
`wip/subsystems-checkpoint/`; never merge that branch into main.
Within the early background-controller group, matching rose from **5.957% to
56.888% (+50.931 percentage points)**. This is a subsystem measure, not the
whole-game percentage.

## October 12 entry point and contiguous subsystem batch

Current matching coverage is **15.199%**, up **0.635 percentage points** from
main's previous 14.564%: **53 new exact functions**, 4,954 total / 11,088.
The additional five inventory entries split verified hidden function bodies;
total tracked code remains 2,415,354 bytes. See
[entry point and menu notes](ENTRY_AND_MENUS.md) for readable subsystem roles.

This batch adds `AgbMain` and missing startup helpers, two curved movement
controllers, script commands, all eight adjacent palette/projection/interaction
functions, most of the sound-test menu, and the remaining scripted-presentation
lifecycle. Readable source aliases and `symbols/proposed/entry-animation.csv`
record provisional names. The **20% target is not achieved**; 4.801 percentage
points remain. No new tooling or regression tests were added.

Working branch: `codex/entry-animation-to20`. Individual commits passed fast
checks. The single combined build printed `build/boktai3.gba: OK`, recorded in
`build/entry-animation-combined-build.log`.
Keep continuation work in contiguous units and use percentages first.

Best failed sources are preserved on
`codex/wip-entry-animation-2026-10-12` under `wip/entry-animation/`. Never merge
that branch into main. In particular, do not repeat the many failed tiny-register
experiments on the sound-test playback indicators or the old spawn-family
candidate without a fresh hypothesis. A two-minute existing-permuter run was
used for the indicator while integration notes were prepared.

## October 12 animation batch

Code coverage is **14.564%**, up **0.785 percentage points** from the previous
main baseline (13.779%). The batch adds **55 exact functions**, with 4,901 total.
The animation work and readable callback roles are in [ANIMATION_081E.md](ANIMATION_081E.md).
Two contiguous ranges are now fully represented by matching C: the state/reset
and child callbacks (`081E5608–081E692C`), and animation/rotation/scale/particle
callbacks (`081E78F4–081E9BA4`). Other movement/dispatch paths remain unfinished.

User-authorized local register pinning unlocked the larger callbacks. Marked
sources are reported using decomp.dev's native Register-pinned category; they
retain exact byte credit. Fast checks gated individual commits; the combined
build printed `build/boktai3.gba: OK`. No repeated emulator/shift runs were needed
for this batch: data and pointer definitions are unchanged.

Prioritize further production matching, not more tooling. Work solo unless newly
authorized otherwise. Use percentages first, defer register-hint cleanup, and
build once at batch integration. Sources for remaining experiments are preserved
on `codex/wip-animation-2026-10-12` under `wip/lean-family/`; never merge that WIP
branch into main. The older pilot and solo budget limits below are historical.

## October 12 measured pilot: completed

The user replaced the solo-only restriction with two isolated Sol 6.1/high
workers, each capped at twenty minutes. Both stopped; no ongoing matching.
[Campaign receipt](campaigns/2026-10-12.json) records results and usage limits.

- Production: **three new exact functions / 1,236 bytes**, completed and
  committed within 14m22s. The snapshot/reset callback family at 080AD270,
  080C6010 and 080D1EA0 shares flag consumption, owner-flag clearing, a saved
  field copy and state reset; only its preparation helper differs. This is an
  observed code role, not an identified character or gameplay action.
- Diagnostic: **zero exact bytes**, stopped after 5m18s and three failed
  hypotheses. Park 08160EA4's register residue until new evidence appears.
- Compact diffs were used in actual matching. Worker feedback exposed duplicate
  compilation for checkpoints; `candidate.py --diff` now prints the same run's
  compact hunks. Smaller output is measured; cost savings are not established.
- Account weekly usage rose from 3% to 6% at the post-worker observation,
  including coordination. Rounded shared usage is not per-worker billing.
- Integration is PR #19; failed sources and reports are separately retained on
  `codex/wip-motion-2026-10-12`, outside production. Research priorities and all
  original user ideas are in [BACKLOG.md](BACKLOG.md).

Next: a small fresh-family campaign, using the saved failed hypotheses to avoid
repeated work. Do not broaden concurrency or build a scheduler on this evidence.
The older solo authorization and allowance floor below are historical.

## October 12 solo workflow update

The user authorized compact diffs, candidate tracking, updated worker guidance,
and a bounded subsystem investigation, explicitly solo. The older allowance
floor below describes the previous tranche, not this authorization.

- [Backlog](BACKLOG.md) preserves the user's complete research/tooling/editor
  ideas; it is not blanket authorization to implement all of them.
- [Candidate workflow](CANDIDATES.md): compact aligned diffs, explicit reviewed
  target extents, retained full output and reproducible per-attempt checkpoints.
- [Motion family](MOTION_0818.md): shared layout and callback chain, three
  existing exact functions using one header, improved unfinished DB10/sibling
  drafts. No new exact byte credit. WIP is separate from production.
- Work selection now favors evidence-backed families and allows useful types,
  naming and architecture work during matching rather than only at cleanup.

## 1. Where things stand

| | |
|---|---|
| Matched C | **4,964 / 11,088 functions (44.77%)**, 369,910 / 2,415,354 code bytes (15.315%; `PROGRESS.md`) |
| Build | `make` / `python3 tools/build.py` rebuilds the ROM **bit-identical** (SHA-1 `2651c5e6875ac60abff734510d152166d211c87c`) |
| Shiftable | all data relocatable; `make shifttest` (3 scenarios, 35 screenshots) passes |
| Code layout | `src/fn/sub_XXXXXXXX.c`, one function per file; `src/lib/m4a.c` (MP2K sound, 57/58) |
| Names | still mostly `sub_XXXXXXXX`; first evidence-based pause-menu leads in `docs/MENU_CONTEXT.md` |

The solo family tranche adds **54 functions / 8,488 exact bytes** on top of
`7037e3a`, following the KNIDL process review. The 20% code target remains
unmet by **150,257 bytes** after the October 12 pilot. [The tranche notes](../notes/solo-family_2026-10-09.md)
record accepted sources, reusable patterns and unresolved candidates.
081DB1D8 contributes only its 116-byte body; two newly inventoried 108-byte
wrappers at 081DB24C and 081DB2B8 are now exact C. Their reviewed replacement
endpoints consume the complete bodies across symbolized incbin fragments.
[The tail ledger](../notes/solo-tail_2026-10-09.csv) records the six tail
matches (752 bytes). The [final ledger](../notes/solo-final_2026-10-09.csv)
adds two more exact functions (312 bytes). The objdiff report now groups eight identified libraries
into contiguous units with verified names; see [reporting policy](PROGRESS_CI.md).

**Historical October 9 authorization: work alone until 15% plan usage remains.** The user
explicitly authorized new matching after the KNIDL review and prohibited
subagents for this tranche. The meter reached **15% remaining** in the final bounded pass; matching
work stopped there, followed only by validation, documentation and preservation.
The tranche is integrated and its sources/checkpoints are pushed; do not
launch another tranche without renewed user authorization.

The preceding existing-draft rescue added 48 functions / 3,724 bytes and
ended with 65 unresolved targets. Its one-hour/15% limit and the subsequent
WIP preservation decision describe that earlier pass.

The [draft inventory](DRAFT_INVENTORY_2026-10-09.md) covers 113 previously
unmatched targets from 435 retained C files (many duplicates or already merged).
48 are now exact; **65 remain nonmatching** after the bounded pass. All exact
source is separately committed through `82d700d` in the reviewed combined
integration; no unmatched draft is promoted. The complete original and restored
ROM builds print `build/boktai3.gba: OK`; all 35 shifted-data screenshots and
five focused boundary/compiler tests pass. Sol workers remain finished; the existing Astra-low worker was
resumed for three capped tasks and produced no new match. No new agents were
started. Source review rejected an invalid permuter candidate.

Nonmatching C and check/permuter outputs remain ignored in local checkouts:
`../wt/push20-integrate/wip/`, `../wt/push20-high-last/wip/`,
`../wt/push20-high/wip/`, `../wt/push20-mid/wip/`, `../wt/push20-low/wip/`,
`../wt/push20-astra-motion/wip/`, and this checkout's `wip/`.
The user subsequently authorized source retention on a separate branch.
[**codex/wip-drafts-2026-10-09**](https://github.com/moozilla/boktai3-decomp/tree/codex/wip-drafts-2026-10-09/drafts)
preserves the original 519 candidate C files plus the solo-family drafts and
alternate experiments, including already-matched drafts. `drafts/targets.csv` maps all 65 unresolved selected candidates to
preserved files; `drafts/manifest.csv` records every source hash. A fresh clone
can recover them by fetching that branch. The original local WIP and a
source-only archive under `build/existing-draft-sources_2026-10-09.tar.gz`
are also retained. No ROM, assembly, dumps or binary build outputs were pushed.

The best large near-match is 08160EA4: 1,060 emitted bytes, four differing
instruction locations. FDB4 has a 300-byte body, not its 788-byte span; its
488-byte tail is a separate body. Other useful near-matches and their selected
source hashes are listed in the inventory. 08235000 is now exact, so the older
handoff's claim that it remains blocked is superseded.

The current policy prohibits nonmatching C on `main` while allowing dedicated
WIP branches outside production `src/`. Candidate source retention is now
implemented by the branch above. Fuzzy/partial progress reporting remains
unimplemented pending further research; preserved source receives zero exact
progress credit. Family/library reuse should be prioritized over repeated
isolated register-allocation retries; one-function commits are bookkeeping,
not a requirement to solve functions independently.

The [KNIDL process review](KNIDL_PROCESS_REVIEW_2026-10-09.md) compares its
multiweek module/family campaigns with our recent run. The solo tranche applied seeded cohorts, typed layouts, compiler diagnostics
and durable WIP checkpoints. Its evidence and limits are recorded in the
tranche notes; the 20% goal remains unmet. No partial-credit metric
or compiler recipe change was implemented by the review.

Historical run details follow; their meter limits and counts describe earlier
stages rather than the current state.

Why bytes trail functions: workers go easiest-first. ~7,000 functions remain;
most small ones (<=40 insns) are done in most ranges, so the frontier is the
41-150 instruction tier, switches, and the logged skip classes below.

History in one paragraph: session 1 built the disassembly (gbadisasm),
shiftable data (46k symbolized pointers), the C pipeline and the first 664
matches. Session 2 added `check.py`, `clones.py`, `permute.py`, found 3,096
hidden functions (data-table seeds), and solved three "impossible" codegen
patterns. Session 3 ran 5-6 Sonnet workers overnight (~1,950 matches).

The local session built and verified the native macOS toolchain, added ROM-free
progress Actions and registered [decomp.dev](https://decomp.dev/moozilla/boktai3-decomp).
The reporting denominator now includes three already-matched SDK boundaries
that the old numerator counted but the old denominator omitted (11,028 report
functions versus 11,025 generated disassembly starts). This is an accounting
correction, not three new matches.

An isolated six-target pilot produced six new matches: Sol 5/6, Astra 6/6,
Luna 4/6 after an explicit continuation. See `docs/MODEL_BENCHMARK.md` for
attempts, elapsed times and limitations. Use Sol as the initial default matcher;
escalate hard cases to Astra and give Luna bounded tasks with explicit retry
rules. No per-model plan-usage/cost measurement was available. This runtime
allows three simultaneous workers alongside the orchestrator. Workers retain
per-function commits; the orchestrator validates and merges batches into main.
Progress Actions run only for main pushes or manual dispatch.

Production round 1 then added **52 new matches**: Luna A 15, Luna B 11,
Sol 26 including a compiler-thunk follow-up. One redundant rewrite was excluded.
The verified `_call_via_r4` alias unblocks ordinary C indirect calls through r4.
Per-worker patterns and failures are in `notes/round1-*.md`; detailed accounting
and limitations are appended to `docs/MODEL_BENCHMARK.md`.

The community-reference pass found real permuter wrapper bugs: symbolic versus
numeric addresses incurred false penalties, retained data inflated scores, and
regex source stripping could erase compact loops. `tools/permute.py --debug`
now diagnoses the base without those errors. It preserves static inline helpers,
requires one target ROM function and creates isolated run directories. See
`docs/COMPILER_REFERENCES.md` for tested GCC patterns, upstream/fork evaluation
and validation. A 60-second trial on `081A55D4` did not solve its remaining
register allocation difference; keep that candidate for future escalation.

Production round 2 added **113 new functions and 14,996 progress-span bytes**:
Luna A five, Luna B 21, Sol 40, main-thread counter family seven and libgcc 40.
Three redundant rewrites were excluded. Sol's bounded permuter trials solved
three targets, and allocator-wrapper null fallthroughs are now matched across
the family. The main thread matched both floating-point runtime units with
`old_agbcc -O2`; 18 hidden library boundaries were added to the inventory.
See `docs/LIBGCC.md`, `notes/round2-*.md` and `docs/MODEL_BENCHMARK.md`.

The user authorized continued work until 20% matched code bytes or weekly
allowance below **35% remaining** (extended from 50%, then 40%, by explicit user requests),
then draining all existing assignments. Check
the account meter before each new round. Two Luna workers began round 3 while
a single requested Astra worker reviewed external development-history claims
and SolDec. Its completed `docs/PROCESS_REVIEW.md` identifies concrete GCL-family
script correspondences, B3-specific decoding differences and a 722-target actor
registry. `tools/soldec_audit.py` supplies bounded, read-only checks; no speculative
engine/compiler claims were adopted. The main thread continues hard matching and reviewed batch integration.
Do not conflate worker commit counts with new coverage.

Production batch 3 adds **97 functions and 22,800 progress-span bytes**:
Luna A six, Luna B four, Sol 40, MGS-focused Sol 25 across two rounds, and the
main thread 22 fdlibm functions. See `docs/LIBM.md` for exact historical source
matches and `docs/MGS_GCL_MATCHING.md` for all MGS comparisons, B3 differences
and remaining candidates. One Sol worker remains dedicated to MGS at the
user's request. No speculative gameplay names were installed.

The user challenged continuing Luna after its lower recent output. Luna B's
next 9-minute round added zero matches and was stopped; its slot now runs Sol
6.1 on retained family seeds. This supersedes the earlier decision to keep
Luna production workers running. No measured Pareto-efficiency claim is valid:
account usage cannot be attributed to individual models, and these rounds have
different targets. Prioritize useful native-byte output over further benchmarking.

Production batch 4 adds 43 functions / 6,142 progress bytes (6,144 emitted):
Sol family rounds 1–2 (13), Sol round 4 (8), MGS round 3 (4), and root's
14 unoptimized SIIRTC routines, two memcpy/memset primitives and two clone
ports. See `docs/RTC.md` for the compiler/bitfield evidence. A reviewed but
unintegrated RTC entry at `08248E70` is now an unmatched progress boundary,
preventing 464 retained assembly bytes from receiving false C credit.
Terminal code alignment is handled once by the splitter. All 35 combined
shift screenshots and 32 tool/context tests pass.

Production batch 5 adds **135 functions / 23,220 progress bytes**: Sol round 5
(15), Sol family rounds 3–4 (10), MGS round 4 (14), and root EEPROM/RFU work
(96). MGS round 5 adds no matches and documents three remaining candidates plus
unsafe permutations that must not be reused. All findings are in
`docs/MGS_GCL_MATCHING.md`. The exact RFU_V1024 source matches consolidate 54
existing functions and add 91; seven ARM interrupt routines use `agbcc_arm`.
Ten reviewed ARM boundaries exclude three retained assembly trampolines from
C credit. See `docs/RFU.md`, `docs/EEPROM.md`, and the ROM-free inventories.
The complete combined ROM is byte-identical; all 35 shift screenshots and
36 local tool/context tests pass.
The user extended the floor again to **35% weekly allowance remaining**. This
supersedes older 40%/50% instructions; finish existing work after crossing it.
Do not recheck the live decomp.dev page; local reports and Actions suffice.

Production batch 6 adds **34 functions / 4,812 code bytes**: Sol round 6 and
short follow-up (10), Sol family rounds 5–6 (13), MGS round 6 (7), and root's
solar timer/IRQ routines plus teardown helper (4). MGS round 7 adds evidence
without new matches. Three independently reviewed retained Thumb entries
(081B61E0, 0821E0D0, 0821E2F8) exclude 340 assembly bytes from the new matches'
progress spans. `docs/SOLAR_SENSOR.md` maps the low-level sensor API and RAM:
four emulator threshold inputs give measured results within one count, with
identical snapshots/probes in the original and rebuilt ROMs. The standard
`tools/emu/harness` has now been rebuilt with the corrected probe source.

**The earlier run stopped at its allowance limit.** The meter reached 35%
remaining, no further assignments began, and all in-flight work was finished;
it showed 34% while integrating the final batch. The final combined ROM
SHA-1, all 35 shifted-data screenshots and all 36 tool/context tests pass. All three Sol workers are
idle with committed work integrated and ignored WIP preserved in their isolated
checkouts. The sustained run from `e2546f6` added **422 functions / 71,970 code
bytes**, advancing 9.302% to 12.282%; it stopped on allowance, before 20% code.
Across all dedicated MGS rounds, 50 functions / 7,024 emitted bytes matched,
with complete evidence and remaining targets in `docs/MGS_GCL_MATCHING.md`.

The new `docs/SCRIPT_TRACING.md` records 698 native calls and 90 actor callbacks
from the intro, verified against command tables and the 722-entry actor registry.
The main thread fixed execution-sample PC adjustment and IRQ timing in the
harness. **Regenerate older execution coverage before entry-level claims.**
Existing memory-hook evidence is unaffected. `tests/script_probes.txt` and
`tools/script_trace.py` provide a reproducible read-only runtime trace.

The deeper task connects emulator coverage to function boundaries:
`tools/function_context.py`, `docs/RUNTIME_CONTEXT.md`, and `docs/MENU_CONTEXT.md`.
Screenshot review corrected a one-tab offset in the menu replay's labels and
separated transition marks from stable screens. Inputs and all 12 screenshots
remain unchanged. Config/sleep/save confirmation handlers are promising naming
leads; an A-button replay is the next evidence step before adopting the names.

## 2. Setting up (new machine/session)

The ROM is never in git. On the current Mac, it is already at `baserom.gba`,
extracted from the user's Downloads ZIP and SHA-1 verified. `build/venv`,
`build/tools`, `gen/`, and a verified baseline `build/boktai3.elf` are ready.
Activate `source build/venv/bin/activate` before using Python tools. Homebrew
provides `arm-none-eabi-binutils` and mGBA; `make setup` builds the pinned
compiler, disassembler, assembler, permuter and harness. See README for macOS
installation instructions. Full `make` and all 35 shift-test screenshots passed
after the production batches were integrated.

On a fresh Linux machine, supply the ROM, then:

```
ln -s /path/to/rom.gba baserom.gba        # SHA-1 above (Japanese, U33J)
apt install build-essential cmake binutils-arm-none-eabi libmgba-dev python3-numpy python3-pil
pip install -r requirements.txt
make setup      # gbadisasm (patched), agbcc, armips, permuter, emulator harness
make disasm     # ~15-20 min: gen/code.s -> gen/code_sym.s + gen/data.s (11,025 functions)
make            # must print build/boktai3.gba: OK
```

`gen/` and `build/` are generated, gitignored, never committed (ROM-derived).

## 3. How to match a function (the inner loop)

```
python3 tools/worklist.py --range 08000000 080492B0 --max-insns 80   # unmatched, easiest first
python3 tools/asmat.py 0800E08C 60      # the asm (runs past the end; stop at the next func_start)
python3 tools/ghidra_c.py 0800E08C      # Ghidra draft, if gen/ghidra export exists (optional)
mkdir -p wip && $EDITOR wip/sub_0800E08C.c
python3 tools/check.py wip/sub_0800E08C.c   # ~0.5 s; side-by-side diff, '!' = differs; ends MATCH/MISMATCH
cp wip/sub_0800E08C.c src/fn/ && python3 tools/build.py   # must print OK, then commit
```

Rules that matter (all in `docs/WORKER.md`, which also has ~40 agbcc tricks):
* Only commit files that MATCH and a build that prints OK. Never hand-written asm.
* Never write raw ROM data addresses (`0x086xxxxx`) in C: declare the
  `gen/data.s` label (`extern const u8 gUnk_08603300[];`). build.py rejects them;
  they break the shift test.
* RAM: `extern T gUnk_02xxxxxx;` (the linker defines it from the name).
* Callees: declare locally with their `sub_` name. Functions that have a real
  name in gen (e.g. `Script_*`, m4a names) must be defined under that name.
* Always read check.py's final MATCH line; a compile error prints no `!` lines.

## 4. Tools

| Tool | What |
|---|---|
| `tools/build.py` | split asm around src/*.c, compile (agbcc -O2 -mthumb-interwork; per-file `// CFLAGS:` override), link, SHA-1 check |
| `tools/check.py FILE` | one file vs ROM at its address, compact aligned diff |
| `tools/worklist.py` | unmatched functions with size/branch/jumptable signals; `--chunks N` to split ranges |
| `tools/clones.py` | clone families: `--stats`, `--port [--loose] [--range A B]` copies matched C to identical siblings (loose = immediates may differ), keeps what check.py accepts. ~6 min for the full ROM. Run after every merge. |
| `tools/permute.py FILE --run SECS` | decomp-permuter for register-allocation misses |
| `tools/progress.py [--markdown PROGRESS.md]` | numbers |
| `tools/disasm.py` / `symbolize.py` | regenerate gen/ (seeds: BL targets, code literals, data-region function tables, aligned gap targets) |
| `tools/shift_test.sh`, `make shifttest` | move data 64 KiB, compare emulator screenshots |
| `tools/emu/harness` | headless mGBA: scripted input, screenshots, coverage, `dump/watch/poke/freeze` |
| `tools/mgba/centaur.lua` | desktop mGBA: input recorder + peek/poke console |

## 5. Running parallel workers (what worked)

* `tools/worktree.sh NAME` -> `../wt/NAME` on branch `codex/NAME`, sharing gen/,
  the ROM and tools by symlink. Existing worktrees: `../wt/s0`..`s5`, `bk`, `disc`.
* Ranges used in session 3 (each has ~1,000+ functions left):
  s0 `08000000-080492B0`, s1 `080492B0-0810ECEC`, s2 `0810ECEC-08182F24`,
  s3 `08182F24-081F612C`, s4 `081F612C-0824DAFA` (skip MP2K `0822F248-08231440`).
* Give each worker `docs/WORKER_ROUND.md` with NAME/START/END. Sonnet-class
  models did 80-135 matches per round at 130-340k tokens. Workers commit
  after every batch, so a usage-limit cutoff loses little.
* Lead merges: `git merge work/NAME`; on add/add conflicts (two workers or
  clones.py matched the same function) keep either side (`git checkout --ours`),
  check for `<<<<<<<`, build, push. Then `clones.py --port --loose`.
* A "seeder" worker is high-leverage: list unseeded clone families
  (see the snippet in the session-3 log: `clones.LOOSE=True; clones.families(...)`,
  families with no matched member, largest first), match one representative
  each, then port siblings. Top families gave 10-50 functions each.
* Run `make shifttest` after big merges; once, a worker's raw addresses broke it.
* Builds slow down to 30-120 s with 5-6 workers on 4 cores; workers batch.

### Helping from another plan / another agent

Pick a range nobody else is working on (coordinate via a GitHub issue or a
branch name), branch from `main`, follow sections 2-3, commit one function per
commit (`match sub_XXXXXXXX`), and open a PR. Every file must MATCH and
`build.py` must print OK on the branch. Good first targets:
`python3 tools/worklist.py --range START END --max-insns 40`, then 41-80.
Skips are logged per range in `notes/s*.md` (with reasons) — a fresh model
solving those is the most useful test of capability.

## 6. Known-hard classes (backlog for stronger models / permuter)

Solved (see WORKER.md): `mov ip, r0` leaves (struct arrays); redundant null
check (`return &p->unk0`); materialised bools (`static inline u8` helpers);
function pointers; switches (case order = asm order); fall-off-the-end returns.

Open:
1. A big constant derived from another constant's register (`movs r2,#0x94;
   lsls; ... subs r2,#4`) instead of separate literals (081A6108, 081A6EF8,
   0813BA24, 0814DF0C). Real struct fields get partway.
2. Alloc/init/free wrappers: solved in round 2 with guarded initialization and
   non-void null fallthrough retaining allocator r0; see notes/round2-sol.md.
3. `movs r1,#3; ands r1,r0` (result in the constant's register): 081F612C family.
4. Stack `s16[3]` vectors whose address lives in a callee-saved register
   (081C43B0 family; 08201CAC's `u16 *q = (u16 *)&v` trick may help).
5. Loops where agbcc hoists or strength-reduces what the original recomputes
   (0811E8B0 family, 080150D0).
6. Pure register-allocation swaps (most remaining skips): `tools/permute.py`.
7. `CgbSound` (last m4a function): stack layout solved, registers left;
   `notes/cgb.md`, best permuter candidate `notes/cgb_permuted_1480.c`.
8. Float/double libgcc units are now matched with `// COMPILER: old_agbcc` and
   `// CFLAGS: -O2`. Remaining libc and integer helpers are library candidates;
   compiler choice requires exact-byte evidence (docs/LIBGCC.md).

## 7. Other open work

* Progress reporting is live on decomp.dev; maintain the ROM-free report and
  batched main-only CI as the source layout evolves (`docs/PROGRESS_CI.md`).
* Naming/context pass: tag functions by the screens they run on (harness
  coverage per `mark` segment), name subsystems, apply `symbols/proposed/*.csv`.
* Cleanup: merge `src/fn/*.c` into real translation units with shared headers
  and structs (contiguous runs); extract assets (graphics, tilemaps, songs:
  classify the 1,483 song-table entries).
* Coverage: record playthroughs with `tools/mgba/centaur.lua`; only ~6% of
  code is exercised by the scripted tests.
* Lua pseudo debug menu (user idea): extend `centaur.lua` with scene jumps,
  flag toggles, warps, sun level, using RAM addresses as the decomp names them.

## 8. Translation (separate repo)

[boktai3trans](https://github.com/moozilla/boktai3trans), branch
`claude/kind-shannon-ugkern`: text tooling (`decomp/tools/text_dump.py`,
`charmap.py`, `text/script.jsonl` with Japanese + 2007 English per string).
Ideas: inserter with box-width checks (1.47 MB free ROM space), `{1F}{xx}`
accents, VWF via `Text_DrawGlyph` (`08218D1C`). Lan Hikari's 0.9 PR there is
still open (recommendation: merge for players, ask for sources).

User preference: after the confirmed live integrations, do not spend browser
work rechecking decomp.dev for each batch. Local reports and Actions results
are sufficient; the user explicitly requested this on 2026-10-09 UTC.
