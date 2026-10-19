# Wireless game items and display (tentative)

Tags: #wireless #matching #types

Range: `080395D4–0803AC9C` (end exclusive).

## Consolidation

Merged the 45 existing matched files into
`src/wireless_game_items_and_display.c`, in ROM order, including the four
unmatched functions at their original positions. No inventory gaps were found.
No functions newly matched; exact coverage gain is **0.000 percentage points**.
No existing match or register-pinned category credit was lost.

The shared header includes `engine.h` and defines the exchange-controller fields,
remote health bytes, item manager, 16 item slots, numeric sprite, animation
drawing, resource header, packed positions and display task. `GameState` is reused
from the existing engine header. Field meanings remain tentative.

Observed GBA layouts:

- Item: `0xB0` bytes; state/entry/flags/count/shake ticks at `0..4`, arc velocity
  at `6`, timer at `8`, vectors at `C/14/1C`, numeric sprite at `24`, drawing at `50`.
- Manager: current/pending effect counts at `18/1C`, animation data at `20`,
  resource header at `3C`, 16 item slots at `5C`; allocation size `B5C`.
- Drawing: flags at `8`, changed byte at `1A`, packed coordinates at `20`;
  size `60`. The position union shares the original two-word copies with the
  halfword coordinate accesses.
- Display: timer/state/entry/finish at `18/1C/1D/1E`, resource at `20`, four
  drawings at `40`, second resource at `1C0`, two extra drawings at `1E0`;
  allocation size `2A0`.
- Exchange: round count at `1E`, state at `28`, defeat flags at `2D/2E`,
  pending event at `30`, item-enable flag at `32`, local counts at `3C`,
  toggle at `42`, remote health at `4C/4D`, display task at `60`, weighted
  total at `DC`, weights at `E2`, cycling choice/latch at `EA/EB`.

agbcc rounds nested six-byte vector structures to eight bytes. `struct Task`
already occupies `18` bytes; adding explicit task-tail padding is incorrect.
The isolated `old_agbcc` selector on `080397C0` is unnecessary: default agbcc
matches this leaf exactly in the merged unit.

## Unmatched functions

- `080399F4`: steady item display (tentative). Shows the count, restores the
  slot position, jitters while shake ticks remain, and starts outgoing items
  or blinking when its timer reaches 30. Retained as assembly; the typed
  trial had substantial register/address-lifetime differences (about 43%
  candidate-length fuzzy). Scratch candidate: `build/unit-wireless/sub_080399F4-draft.c`.
- `0803A068`: scattering item motion (tentative). Starts at the remote side,
  computes a randomized direction, follows the arc for 28 frames, then adds
  a drift vector until frame 44. Retained as assembly; random-load ordering,
  arithmetic and register allocation differ (best trial about 73% fuzzy).
  Scratch candidate: `build/unit-wireless/sub_0803A068-draft.c`.
- `0803A514`: `WirelessGameItems_QueueLocalItem`, guarded draft, **93.0% fuzzy**
  recorded with `check.py --nonmatching --record`. Flushes an occupied local
  transition slot, clamps the new count and projects the supplied world vector
  relative to `gUnk_03005418`. The clamping zero temporaries/register choices
  still differ. The default build uses assembly.
- `0803AB28`: `WirelessGameDisplay_Init`, guarded draft, **85.8% fuzzy** recorded
  with the same command. Loads resource contexts `7C8B` and `5D04`, initializes
  four notices and two extra animations, then resets state. Loop pointer
  lifetimes/register allocation still differ. The default build uses assembly.

Approximately 25 draft-check iterations, plus per-function checks, layout checks
and the combined-unit exact checks. No new register pins or barriers. Existing
pins were audited with `tools/depin.py --write`; none could be removed.

## Local adaptations and API observations

Some address temporaries must remain to preserve agbcc allocation: direct
`items[index]` expressions changed exact output in the merge and outgoing
handlers. Their offsets now derive from the shared fields and `sizeof`, rather
than duplicated layout constants. The selected equipment lookup similarly adds
the selected index to the object before adding the shared `slots88` offset;
direct array indexing changes instruction order. Existing pinned drawing-offset
expressions derive from shared fields too. The item toggle uses a byte pointer
into the controller representation so accessing its mode byte does not index
beyond the four-element counts array; direct field access changes allocation.
No alternate partial structs remain.

The task factory calls deliberately preserve the two argument registers passed
to `WirelessGameItems_Create`; its body does not use them. Engine APIs are taken
from `engine.h`, with no local redeclarations. `Task_SetCallbacks` declares void
callbacks, whereas these matched task handlers return `int`; no casts or engine
header changes were made to hide that signature mismatch. The engine ignores
these callback return values (`TaskSystem_RunFrame`, `src/fn/sub_0821A054.c`).

`merge_unit.py --write` normally removes inputs with `git rm`. The worker used
its merge implementation with working-tree-only input removal, preserving the
driver-owned index. No commits, git metadata edits, generated source edits or
shared documentation edits were made.

## Validation

- All 45 original C functions passed individual exact checks after type unification.
- The merged default unit passed `tools/check.py`: **0 differing bytes**, full
  `0x16C8`-byte assigned range, including retained assembly.
- `make format` and `git diff --check` passed.
- `tools/progress.py`: matched function/byte counts and register-pinned credit unchanged;
  two additional guarded drafts, which receive no exact credit.
- Combined ROM build: **`build/boktai3.gba: OK`**. The first attempt exposed
  missing local `build/res` inputs. Eleven generated data sources were extracted
  locally and checked against the ROM; indexed archive filenames required by
  the shared assembly were materialized locally. No shared `gen/` files changed.
