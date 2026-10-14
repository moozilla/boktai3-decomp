# Scene pool and actor controls, 0803CDF0–0803EE58

This contiguous campaign follows the scene-object controller at 0803BAA0.
Its gameplay identity is still tentative. Exact C rises from 22.309% to
22.381% (+0.072 percentage points): ten new functions, 1,732 bytes.
Of those bytes, 288 need no register pinning and 1,444 use the authorized
concession. `tools/depin.py --write` removed all unnecessary hints first.

## Observed ownership and fields

`include/scene_pool.h` describes a manager allocated by 0803D5A8 (task list 8,
size 0x2360). Its initializer 0803D514 allocates a configurable number of
0x17c entries, default 0x60, and initializes four inline 0x8b0 actor slots.
The manager is stored at gUnk_02000108. These are static source/assembly
observations, not new runtime coverage or identification of an enemy type.

Manager offsets: capacity +0x18, active count +0x1c, pool pointer +0x20,
seen-ID words +0x24, actors +0x44, player mask/count +0x2304/+0x2308,
region words +0x230c, actor bounds +0x2314/+0x231c, position snapshots
+0x2324, registry +0x2354. The update reads up to two adjacent player pointers
at gUnk_020004F0, stops at the first null pointer, and extracts regions from
position bits 24–31. Its loop remains unmatched.

Entries contain script ID +0x0c, pool index +0x10, three 12-fractional-bit
position components +0x14, active/kind +0x24/+0x25, side/mode/repetition
bytes +0x38/+0x39/+0x3a, state-frame counter +0x44, motion parameters
+0x48–+0x54, signed metric +0x58 (field `direction`, tentative), idle counter
+0x60, entry flag +0x64, steering active/delay/pulse +0x65–+0x67, state
number +0x68 and a 0x100-byte history buffer at +0x6c. Settings and delays
at +0x16c–+0x178 are set by the already matched adjacent RNG helpers.

The actor view is local to this family. Controls at +0x858 affect speed
fields +0x5ac and +0x5b4; a +0x200 control sets two 500-valued halfwords
at +0x58/+0x5a and calls 0811EDE0. The interpretation of that action is
unconfirmed. Negative vertical speed (+0x5b0) or a nonzero shifted metric
(+0x38) blocks these controls.

## Exact additions

| Address | Provisional name | Bytes |
|---|---|---:|
| 0803CDF0 | ScenePool_ParseEntryParameters | 248 |
| 0803D1E8 | ScenePool_ReadActorBounds | 40 |
| 0803D61C | ScenePool_CreateEntryFromScript | 100 |
| 0803D680 | ScenePool_SelectInitialDirection | 128 |
| 0803D700 | ScenePool_SelectRepeatOrExit | 140 |
| 0803D78C | ScenePool_SelectRepeatedDirection | 128 |
| 0803D80C | ScenePool_SelectDirectionOrRepeat | 156 |
| 0803D8A8 | ScenePool_ResumeAfterIdle | 100 |
| 0803D90C | ScenePool_ApplyMotionControls | 204 |
| 0803DDA8 | ScenePool_StateCruise | 488 |

The parser reads n/t/a/l, optional s and p, and r (default 20000). The
optional position vector's real struct shape eliminated every matching hint.
Entering a state resets +0x44, sets +0x64, and clears the history buffer.
Cruise control selects speed flags using the signed metric, sometimes schedules
lateral pulses from the shared RNG table, tests exit/idle conditions, then
selects one of four kind-specific transition helpers unless player flag 2 is
set. This is the first state in a larger callback family; the subsystem is
incomplete.

The parser and entry-creation success path retain the original unused r0
behavior. Only the allocation-failure -1 is established as a meaningful return;
no success return value or gameplay role is claimed.

## Matching evidence and continuation

All ten functions passed resolved-byte checks with independently reviewed full
extents. A 45-second permuter run solved the two register-copy differences in
0803D90C. The accepted transformation uses an initialized local pointer and its
address; it changes neither object lifetimes nor behavior. A zero search score
was checked independently before acceptance.

0803DC60 remains an 83.6% NONMATCHING draft, recorded with check.py. Bounds-check
materialization, narrow RNG loads and pointer allocation remain different.
The manager update at 0803D430 remains only 72.7% similar in local ignored WIP;
loop rotation and high-register pointer allocation remain. It is not promoted.

Next contiguous targets: 0803DAC8 and the steering helper, then 0803DF90,
0803E1AC, 0803E354, 0803E4D0, 0803E5F4, 0803E7B4, 0803E998 and 0803ED84.
The manager's larger actor update (0803CEE8) and bounds/registry scan (0803D288)
remain assembly. Scratch candidates live under this worktree's ignored wip/;
no new WIP branch is needed. Keep this range until its state family is understood.

The integration build result is recorded in docs/HANDOFF.md after validation.
