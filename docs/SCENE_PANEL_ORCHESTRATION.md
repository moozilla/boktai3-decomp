# Scene panel initialization and script controls

The reviewed range `0805C310–0805C680` contains eight functions following the
sprite/configuration cohort and preceding the command-sequence runner. It uses
`scene_panel_orchestration.h` and the existing `ScriptUtilityReference` layout.
Additional panel fields extend the local interaction view without changing
its existing offsets: initial mode at `0x1A`, resource scale at `0xBF8`, the
main-sprite skip byte at `0xC0F`, and saved presentation mode at `0xC11`.

`0805C310` dispatches on the initial mode. Mode zero first calls the predicate
at `0805C2DC`, which requires every subject's state to be zero. When it passes,
including an empty set, the initializer calls `0805BCEC` and selects interaction
state zero without consuming keyword `O`. Otherwise `O` supplies an optional
integer, defaulting to zero. A nonzero option and nonnull tether set tether
state twelve, copy the panel center into its position and clear its heading.
An active subject (state four) then selects interaction state four; otherwise
state three. That path does not call `0805BCEC`. A zero option or absent tether
calls `0805BCEC` before selecting state three.

Mode one initializes the rig and calls configuration helper `0805C084`. It sets
tether state twelve, copies center/clears heading, and writes the cached mode
one to tether byte `0x1B`. Tether kind six selects resource scale 80, kind seven
selects 120, and other kinds select 100. It initializes the background, overlay
and approach sprite, then selects interaction state eight. Tether presence is
assumed in this path, matching the original accesses. Other initial modes do
nothing. `0805C454` reads optional lowercase `m` into the initial-mode halfword;
an absent keyword stores zero.

`0805C470` guards the global panel pointer, sets its main-sprite skip byte,
reloads the global, and copies current presentation mode to saved mode. This
records the initialization skip observed in `0805BCEC`; no broader hide or
visibility guarantee is asserted.

`0805C4A0` exports six script references: initial mode, tether-at-state-twelve
boolean, then four subject slots. `0805C564` exports the last five values.
Both return without reading script arguments when the global panel is absent.
Each eight-byte reference is decoded by `0821AAD8`, then written at element
zero through `0821B4C8`. A null subject or subject state zero/one writes zero;
state four writes four; every other state writes two. Exactly four slots are
queried, independently of the current subject count. Globals and slot pointers
are reloaded for each output after its reference decode. Earlier reference
writes can update script-accessible state.
The separate null-tether and other-tether branches are preserved.

`0805C614` enables the arrival ramp at `0x1337` and resets its halfword counter
at `0x1338`, reloading the panel global between writes. `0805C640` and
`0805C660` respectively set and clear gate byte `0xC12`. These guarded controls
couple to the state-seven ramp and state-eight transition gate documented in
`SCENE_PANEL_INTERACTION.md`. Their return ABI is void.

All eight full reviewed extents are exact. Three new functions add 696 bytes,
all ordinary C, or 0.0288 percentage points. Five already-C functions contribute
184 typed bytes with no new credit. The mode initializer's two exploratory
bindings were removed by minimization. No pins, empty barriers, instruction
bodies, or explicit r7 bindings remain in this cohort. Full combined ROM
validation belongs to the integrator.
