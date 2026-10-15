# Scene panel scheduled actions (tentative)

0805907C–08059428 contains eight functions /940 bytes. Six new exact bodies
add908 bytes /0.0376 percentage points:600 without hints and308 with minimized
concessions. Two existing wrappers (32 bytes) receive shared typed contracts.
The scheduled-action view is independent of the state callback at250 and effect
state at4FC, while belonging to the same larger ScenePanel object.

Fields are origin500, resource selector133A, frame134C, interval134E, step1350,
resource ID1352, angle1356, action callback1360, handle1364 and signed parameter
13C4. The generic runner invokes a nonnull callback through0824923C, retaining
the existing engine trampoline contract. The no-op action is an empty leaf.

Resource selectors0..4 create/select these actions:0 leaves a null handle and
periodically creates AnimatedHazard at GameState's saved XYZ, with resource ID
and argument30;1 creates an081A3710 handle with arguments10/30 and runs an angle
burst;2 creates0819AD94 at origin and calls0819ADF4 periodically;3 creates08190704
with the signed parameter and calls08190B10 at origin with ROM data0824DEA4;
4 creates0819AA8C at origin and periodically calls0819AAD8. Intervals are600,
except selector3 uses360. Other selectors clear the callback. This initializer
retains the original treatment of existing counters and handles.

Ordinary periodic actions increment a halfword frame and compare its wrapped
value with interval, then call the action and clear step/frame on expiration.
The angle burst waits until its increment reaches interval, computes
(Math_Atan2(savedX-originX,savedZ-originZ)-32)&255 and clears step. On following
ticks it plays sound994 at step0 and issues five081A376C calls, each with origin,
angle,12 and100, advancing angle by16 modulo256. A subsequent tick resets frame.
The selector3 action also performs an observed Math_Atan2 call whose return is
unused before invoking08190B10; this apparently redundant call is preserved.

All eight complete reviewed extents match. Scheduled-action setup preserves the
shared callback-store control flow with scoped function-pointer bindings and a
cached handle-field offset for the adjacent interval address. The minimizer does
not recognize those function-pointer declarations; their necessity was checked
individually as well. These are temporary concessions, with no explicit r7
binding or instruction bodies. Hex addresses/offsets above omit0x; timings and
byte counts are decimal. No runtime coverage or final in-game identity is claimed.
