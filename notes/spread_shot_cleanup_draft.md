# Spread-shot cleanup and matching evidence

Tags: #matching #scene #particles #structs #compiler

The inspected draft is now implemented and checked. Commit request 0036 consolidated the sprite-notice range 08122DF0–08123194 into `src/sprite_notice.c`; request 0037 consolidates 08123194–08125B64 into `src/spread_shot.c`. Both complete units match the stock compiler. Matching coverage is unchanged.

## Shared layouts

The notice pool uses `Task`, `SceneParticleEffect` and `DisplaySequenceState`. Its 32 slots have stride 0x48, start at 0x20, and precede the occupied mask at 0x920. State, signed slot index and entered flag are at 0x42, 0x43 and 0x44. The position copy matches with the shared position type.

The spread-shot record has stride 0xE4: `SceneActorBody` at 0x00, `ContactSwarmSprite` at 0x48, position at 0x74, velocity at 0x7C, and `ContactCollider` at 0x84. Hit count, kind/reset, cell, signed slot index, state and timer occupy 0xD8 through 0xE1. The two-slot task starts its entries at 0x18 and stores its allocated mask at 0x1E0. The step's display position is `body.position`; the constructor's collision ID comes from `body.id`. Collider bounds and offsets now use `ContactPosition`, replacing the private bitfield vector.

Terrain lookup reuses `SceneActorChaseTerrainCell`, `SceneActorChaseTerrainGrid` and `SceneActorChaseTerrainOverride`. Named attributes, flags, height, override properties and coordinate components replace constant byte-offset accesses. Computing the cell's layout-derived offset before loading the map buffer preserves the original ordering. Materializing a grid pointer first hoists the buffer load and changes hundreds of inlined instructions. The layout-derived address expression preserves the required ordering without duplicating a numeric header size.

## Exact checks and retained forms

- `tools/check.py src/sprite_notice.c`: all 11 functions MATCH, 932 bytes.
- `tools/check.py src/spread_shot.c`: all 13 functions MATCH, 10,704 bytes; final artifact `build/check/run-kiyi77q4`.
- The new spread-shot header has only its own source as a consumer; existing shared headers were not modified. Concession lint reports no findings.
- The terrain probe retains its function-scope cell and signed/unsigned height round trip. Their GCSE/PRE implications are documented in `notes/cloud-08122DF4.md`.
- Sharing the two height helpers changes the step's code (12 realigned instruction differences, candidate four bytes shorter). The matching private helper remains.
- Changing notice interpolation's failure branch to an early return reverses its branch polarity. Its common failure label remains.
- Notice callers pass an unused third argument in the ROM. A local function-pointer call adaptation retains those argument instructions while including the canonical two-argument notice declaration. The between-position caller also retains the original byte-sized result test.

## Scope at closeout

This completes the in-progress spread-shot item. Scene moving particles 08125B64–08126140 remain unstarted. The earlier cell-gate, scheduling-manager, palette-fade and challenge-record experiments remain set aside with their original matching sources restored. No new queue item is started at closeout. Scene status indicator at 08126650 and all later units remain assigned to the cloud worker. Combined ROM validation and report regeneration belong to integration.
