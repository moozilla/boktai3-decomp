# Entry point and contiguous subsystem batch

This batch raises tracked code matching from **14.564% to 15.199%**
(**+0.635 percentage points**, 53 functions / 15,332 bytes). The requested
**20% remains unfinished**: another 4.801 percentage points are needed.
No tools were changed. Individual functions were checked with the fast checker;
the combined ROM build printed `build/boktai3.gba: OK`.

## Game startup now has matching C

`AgbMain` is the main C entry point called by the ARM startup code. Its matching
source now shows the startup sequence, with readable local aliases for the
identified initialization calls:

1. Configure ROM access timing; preserve two reserved EWRAM words across reset.
2. Clear IWRAM, initialize the RTC/clock and audio, and install interrupt handlers.
3. Copy the executable IWRAM block, select game-state storage, and initialize
   the task scheduler, graphics and resource systems.
4. Enable the additional interrupt, calculate solar times and start game state.
5. Run the task scheduler forever.

The new `InterruptsAndVideo_Initialize` installs the interrupt vector and clears
VRAM, sprite memory and palettes. `GameState_SelectRamBase` chooses a small
Timer-3-dependent displacement and assigns pointers to the game-state regions.
`Boot_ClearReservedWords` is a separate four-word clearing helper recovered from
what the disassembler had treated as the entry point's trailing bytes.

This completes the direct startup callees that were still missing C. The ARM
reset/interrupt code remains assembly; this is not a claim that every subsystem
reachable through the scheduler is decompiled.

## Animation, palette fades and interactions

The animation extension contributes **28 functions / +0.384 percentage points**.
Two substantial movement controllers now have matching C: a curved move back to
a resting point, and a curved move toward a scripted target. Both update facing,
frame timing and position and notify a waiting caller on completion.

The eight adjacent functions from `ActorPalette_BeginFade` through
`Actor_SelectNearestInteraction` form a fully matching block. They cover:

- Forward and reverse interpolation of the actor's 16-color palette, including
  restoring its resource palette and notifying completion.
- Projection of world coordinates to screen coordinates and display clamping.
- Interaction eligibility based on distance, facing, height and input.
- Selection of the nearest eligible partner and its animated selection marker.

The actor identity remains tentative. These are observed controller roles,
not names for the game's entire animation system. Script-command matches also
configure position, modes and waiting handles. Four independent command bodies
were separated from a preceding assembly span so their credit is accurate.

## Sound-test menu

The neighboring menu is identifiable as a sound-test screen because it selects
IDs from three sound tables, checks playback through the sound manager, stops
the previously selected sound and plays the new one. Its **13 new matches add
0.116 percentage points**.

Matching C now covers asset/sprite initialization, category counts, the category
cursor, track-name lookup, two numeric displays, play/stop controls, previous/next
selection with wrapping, accelerated left/right repeat, the exit fade and sprite
cleanup. The exact category labels have not been established; the source does
not guess that all three categories are music, effects and voices respectively.

`SoundTest_UpdatePlaybackIndicators` is the sole remaining function in the
contiguous `081E1AF4–081E269C` body range. Its candidate describes the behavior,
but is **not exact and receives no matching credit**.

## Scripted presentations

Eight new functions add **0.110 percentage points**. Together with existing
matches, `081E2898–081E34C4` is now covered by matching C: allocation, layout
selection, frame ranges, initialization, updates, drop/shake/blink effects,
skipping, completion notification and cleanup. The smaller preceding timed
presentation controller also matches.

## Names and remaining work

Provisional names are recorded in `symbols/proposed/entry-animation.csv` and used through
readable source aliases without changing linker symbols. The preprocessed compiler
input and function identities were checked unchanged when applying these names.
The names are practical descriptions and may be refined; no naming investigation
was required before accepting a match.

Remaining animation candidates include directed movement, controller creation,
child visibility dispatch and idle timing. Best source candidates are preserved
separately on `codex/wip-entry-animation-2026-10-12`, outside production `src/`.
Do not merge that WIP branch or count its drafts as matching C. Removing local
register hints is deferred; marked exact sources retain their normal credit in
the report's Register-pinned category.
