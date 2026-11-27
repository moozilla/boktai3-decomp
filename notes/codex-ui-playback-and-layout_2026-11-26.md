# UI layouts and playback recovery

Tags: #ui #types #matching #phase3 #tooling

The dialogue interpreter, text window, and manager use shared layouts of
0x110, 0x1CC, and 0x20 bytes, respectively. The manager keeps the tag used
by its RAM declaration. Scene HUD callbacks use the existing task fields,
and the pause and storage icon paths use the measured AnimSprite stride.
The worker reviewed 47 record families, including the shorter views and
the private embedded types that need more evidence.

The struct merger can forward-declare a named private record reached only
through a member pointer. For an independently reviewed short view, pass
`--allow-prefix SRC` for that unit. The rewrite checks that the tag is
used in direct pointer parameters and that references use members, null
tests, or forwarding. It also maps a complete aggregate member onto a
measured element of the same type in a shared array. Compiler fixtures
cover genuine by-value dependencies, unrelated same-name parameters,
array identity, and stride-sensitive uses.

## Tutorial playback

The controller's renderer sprite and command state at 0xDDC and 0xE24
are DisplaySequenceSprite and DisplaySequenceState. Sprite_Init writes
list links through the last eight bytes of the sprite. The scene body at
0xE80 and saved position at 0xEE0 use the existing scene types. The
controller still occupies its allocated 0xFA4 bytes. Ghost independently
uses the older ActorAnim and ActorSprite declarations.

Seventeen callbacks reuse the mode-and-phase transition, which captures
the old mode and preserves playback when returning to mode 3. Fifteen
call the shared typed playback step. The playback definition lives in a
narrow header so the tutorial can include it without pulling in the
display setup declarations. The previous header still provides those
declarations to its existing clients.

- At 081E394C a direct entry flag changes the immediate's expansion order
  relative to the flag address load. The full state transition matches.
- At 081E59CC the shared playback candidate assigns the completion byte
  to r1. The diagnostic `AGBCC_FORCE=394:2` confirms the ROM's r2
  assignment. The callback retains its existing step.
- At 081E682C a typed child walk forms the correct callback-stride
  address. The count loads on the other side of that address formation.
- At 081E7F80 the ROM loads 10 before the four impact bursts and keeps it
  across eight radial calls. Writing it directly at all 24 operands
  changes the allocation of 4 and 60.
- At 081EA4DC a complete shared initialization view matches the
  initializer with an explicit saved-palette pointer. Flattening both
  palette banks changes induction and register allocation in the earlier
  fade loops, starting at 081E3550. The unit keeps the separate palette
  view and shares the scene body.

## HUD initialization

At 08176F70 the initializer uses common animation widgets, resource
records, and renderer sprites. Ten calls take literal playback defaults.
The first still needs the original binding. Writing all defaults
directly changes the two early zero assignments from r5 and r6 to r4 and
r5, according to Scope's local allocation trace.

UI steering debt fell from 550 to 436 constant uses and weighted warnings
from 577 to 463 through the HUD pass.

## Modifier previews, play statistics, and the map

At 08040610 the eight comparison tile calls use direct corner and palette
values. At 081C0CE4 the results page uses the complete phase-entry
transition. PlayStats_SetState also sets its final pending flag directly.
Direct event consumption in the results cases changes the test and clear
sequence, so the explicit operation remains there.

The map callbacks at 0816B958, 0816BA90, and 0816C2FC use SceneHudState
for the navigation pages, viewport bounds, and position sprites. All
115 dependent units match.

The full worker batch leaves 418 constant uses and 445 weighted warnings,
with zero errors. Stock checks match all 58 changed UI units. The final
controller header passed all 41 dependent checks, and the playback
header split passed all 28 checks before the UI adopted it.
