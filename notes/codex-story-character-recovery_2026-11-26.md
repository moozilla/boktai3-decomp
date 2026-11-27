# Story character layouts and state resets

Tags: #actors #types #matching #phase3

Story character units went from 75 to 57 diagnostic lines and 114 to 75
constant uses. All changed units match the stock ROM. The linter reports
zero errors in the story character sources and headers.

## Actor and display layouts

The 42 short animation state views from 081C6BA8 onward use the manager's
allocated `SceneActor`, including the display animation at 0xD4. The
story callbacks clear and advance the word at 0x10, which is now the
state frame field in the shared layout. Naming the phase and frame
fields allowed literal resets in the four short states at 081C6BA8,
081CB940, 081CBD6C, and 081CC004. Converting raw field macros also removed
literal diagnostics on ordinary reads of phase, speed, and direction.
All six existing manager header dependents matched when the word was named.

The palette pulses at 081CBA04 and 081CBA80 use sixteen colors at 0x1CC
in the state actor overlay. Both can set their entry, phase, and frame
fields to zero directly. The overlay declaration kept all 102 dependent
units exact.

The script placement and command functions at 081CE1BC and 081CE6DC,
and the cutscene dispatcher at 081CD484, share the actor's script view.
Its effect state starts at 0x20, body at 0x5C, display at 0xD4, event
receiver at 0x168, and work palette at 0x1CC. The new header checks the
0x2E8-byte extent through the callback slots. The allocated slot extends
past this view. All nine dependents checked so far match, including the
palette fade at 081CDE14 and the kind-specific body offset at 081CE0E4.

## Remaining compiler decisions

- At 081CE0E4 the ROM addresses the body at +0x5C, then stores the position
  offset at +0x34, +0x36, and +0x38 relative to that pointer. Taking the
  address of the embedded position instead yields a base at +0x90 and
  stores at +0, +2, and +4. The typed body pointer recovers the original
  form for the height setters.
- The attack effects at 081CB3F4 and 081CBE30 retain an advancing halfword
  pointer into the position. Direct member reads use a stable base with
  register offsets in agbcc. Both effects use the typed display and
  direct phase and frame resets. The facing animation at 081CB6AC also
  uses a typed tag and direct resets.
- At 081CC540 the display mask precedes the halfword load. A direct
  typed OR reverses that order and changes the Thumb instruction count.
  The same mask-order decision appears in the neighboring blink and
  display states. The recurring multi-field player-facing predicate at
  081CC1B4 has counterparts in the manager and Kuro.
- The direction handler at 081C6780 holds its entry flag across dispatch,
  and constructs the random target height between its coordinate
  additions. The direct variants change allocation and instruction order.
  At 081C6954 inlining failure returns changes the cold path and the
  allocation of the name and model across calls.
- In the command dispatcher, a common height store after the tag branch
  shortens the function by four bytes. The literal-height variants also
  change the order of the halfword stores and constant setup. The two
  placement entry points retain the state and default argument lifetimes.
