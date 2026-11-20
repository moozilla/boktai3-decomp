# Scene actors and NPCs, second half

Tags: #scene #actors #matching #tooling

The November 19–20 worker reviewed 25 units through 081C3010. Reported
warnings fell from 417 to 205 across those units, a 50.8% reduction.
The full assigned queue fell from 657 to 445, a 32.3% reduction. This
includes two linter false positives and earlier parameter-helper matches
that still need source review. The worker report identifies those helpers
and their callers. No warning exemptions were added. Every changed
translation unit and each checked header dependent matched the stock ROM.

The paired status and presentation callbacks now use the common
`PairedSceneHalf` and `VanargandAttackPair` layouts. The scroll and contact
effects share clamped signed interpolation with eight fractional bits.
The contact effect uses that operation for both its position and scale
ramps. Actor controller callbacks use the shared collider types and the
two repeated collider profiles.

Open examples include 08122338, where the random-table address needs to
stay live across three divisions, and 081C069C onward, where direct
waypoint field writes coalesce the ROM's halfword stores. The local
report records the other open decisions and the exact source forms
retained for them. The coordinator stopped the worker before
081C66F8, leaving that unit and the following 11 units for the next pass.

Scope now suggests a valid diagnostic when a diff contains moved
instructions. The concession linter now reports local unions only
within functions. Both changes have focused test coverage.
