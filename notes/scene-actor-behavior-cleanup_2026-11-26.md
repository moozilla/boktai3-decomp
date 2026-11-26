# Scene actor behavior cleanup

Tags: #matching #types #phase3 #scene

The actor behavior pass reduced diagnostic lines from 186 to 163 (12.4%) and
charged constant uses from 190 to 166 (12.6%). The scoped linter reported zero
errors. All touched translation units match under the stock compiler. Exact
code coverage remains 100%, a change of zero percentage points.

## Source recovery

- The behavior callbacks (0812F12C, 0812F270, 08131B40, 08131BC4, 08131C48)
  use the engine's combined scene and engine pause query. The behavior
  header retains an alias for the existing Skeleton caller. All 123
  dependents match the ROM.
- Terrain script selection (08078160), contact reaction (08078854), pending
  commands (0812E7EC), and route recovery (08131044, 081314E8) now use
  the owning terrain, collider, actor, or route types.
- The four GameState halfwords beginning at +0x884 are indexed by player
  slot in both actor record routines (080789EC and 0807F4B8) and in the
  indexed increment (08227E5C). The defeat-record caller now uses this
  typed array. All 2414 dependents of the GameState header match. The
  nearby +0x530 counter is still accessed through its observed offset.
- The status update (08078EE8) uses the shared delayed response,
  a common pending response transition, and the real tracking component
  in its byte flag clears. The shared tracking implementation and all 21
  header dependents match.
- The target and route recovery callbacks (0812EED8, 081301C4) use the
  existing delayed response initializer. Pending reactions, the idle
  callback, and three route/control callbacks reuse the same control
  install or state entry operation.

## Remaining compiler decisions

- 08078DEC still has a raw pending-control clear. Using the field directly
  changes the flow and allocation after clearing its pending flag.
- 0807A62C still reads the timer and unregisters the contact through
  offset views. Using the typed fields changes the computed addresses
  and the related register allocation.
- 0807AF94 and 0812F5D4 still use separate four-attempt random-heading
  loops. The shared helper changes the sample and loop register homes.
- 0812EED8 still returns the tracking pointer along with the test.
  Capturing it separately and using the shared predicate changes
  its long-lived allocation and the subsequent reload choices.
- The sibling pause callers keep their independent external flag tests.
  In 0812F12C, the ROM makes the mask before loading the flag address.
  The direct field expression reverses those instructions. The remaining
  direct field and mask trials are listed in the worker report.
