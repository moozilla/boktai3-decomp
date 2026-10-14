# Contact actor pool setup and display (tentative)

Reviewed subsection: **080427FC–08042E18**, with seven new exact functions,
**0.047 percentage points** (1,144 bytes). Combined progress is **23.049%**,
6,359 / 11,465 functions and 556,706 / 2,415,354 bytes. Without register pinning:
**18.990%**; Register-pinned: **4.059%**, included above. The initializer is a
guarded **88.6%** draft and receives no exact credit. There are 69 drafts overall.

## Recovered layout

The manager owns **sixteen 0x16C-byte slots**, starting at manager +0x3C, with
shared tile data at +0x1700. Its state is +0x18, clear request/mode bytes are
+0x1E/+0x1F, resource/health collider IDs are +0x20/+0x22, two target-position
pointers are +0x28/+0x2C, and script state starts at +0x30. The complete manager
allocation/tile buffer length remain unconfirmed; the header is a known prefix.

Each slot contains entry/state bytes +0/+1, finished/bob-phase bytes +2/+3,
state-frame halfword +4, direction +6, signed target index +8 and retarget timer
+9, target height +0xC, speed +0x10, and a word-aligned packed 8-byte position at
+0x14. The first view incorrectly treated position as three words; movement
accesses prove it is three signed halfwords plus padding. Existing matched
consumers were rechecked after this correction.

The resource collider starts at +0x2C, health collider at +0x80, sequence state
at +0xD4, sprite at +0xE4, graphics at +0x110 and shadow at +0x12C. `active` was
also an incorrect first interpretation of byte 0: state handlers clear it on
entry, and transitions set it. It is now named **enter**. Byte +8 is a target
index, with negative values meaning no selected target; the tracking routine's
access to the two target pointers establishes this meaning.

Gameplay identity is still tentative. These offsets, operations and state
transitions are observed code behavior, not an execution-coverage claim.

## Contact behavior

Health contact takes a direction from source-collider flags bit 0x100 / byte
+0x42, otherwise computes it with Math_Atan2 from the collider X/Z differences.
If the slot already has a target, it sets a 250-frame retarget timer and target
0. It processes contact, conditionally clears the other collider's cost from
actor/player modes, passes a (16,0,16) vector to 080472A4 and applies the observed
health-cost helper. It sets state 5, enter 1 and state-frame zero.

A zero cost plays checked sound 0x211. A nonzero cost plays 0x108 and, when source
reaction flags bit 1 is set, advances the 1024-entry random-table index and
spawns the effect described by gUnk_0824DE9C. The callback uses the checked
`RANDOM_TABLE_ADDR` constant from Claude's register-allocation work. Resource
contact uses 080472A4 with argument 1 and the same vector, then calls the
resource-cost helper. The cost helpers' gameplay identity is not inferred here
beyond their existing provisional names.

## Initialization and hiding

The readable initializer draft establishes graphics entry DA6D, tile 2D3 and
shared tile data at manager +0x1700. It initializes the sprite and shadow, zeros
the two sprite-position words, and constructs both colliders with bounds
(32,240,32), offset (0,120,0) and the slot position. Health contact uses flags
0x4005, health ID, callback 080427FC and options 0x82/1; resource contact uses
flags 0x2001, resource ID, callback 0804293C and the observed additional options.
These interpretations are supported by the original assembly, despite the draft
not matching yet.

Hiding sets sprite flag 1, health-collider flag 4 and disables the shadow. The
pool helper hides all sixteen slots, invokes the +0x30 script-state helper and
clears manager state. The slot's hide helper retains an unspecified success
return; observed callers ignore it, as documented in the matching playbook.
The already matched destructor was named `ContactActor_DestroySlot` without new
coverage credit. It removes the health collider, calls the shadow cleanup helper
and removes the sprite; the later manager destructor loops over all sixteen.

## Display and advance behavior

Directional sequence selection maps `(direction + 96) >> 6 & 3` to two flip
bytes, then calls Anim_SetSequence with the requested halfword index and the
manager's sequence resource at +0x38. The two bytes live on the stack in the
original, and both are initialized before use.

Display update copies world position into sprite position, then runs the same
packed sequence timing used by the adjacent display wrapper: frame in command
bits 6–15, flip bits 4–5, duration weight bits 0–3, reverse traversal and endpoint
hold/wrap. It stores finished at slot +2. It then moves world Y toward target
height by four units or snaps when close, adds a sine-derived bob offset to
sprite Y with truncation toward zero, and advances bob phase by four. **The copy
precedes the world-Y adjustment**, so the sprite uses the previous world Y in
that update. Coordinate units remain unconfirmed.

Advance clears enter, moves X/Z using the sine/cosine table and twice the speed,
with division by 4096 truncated toward zero, refreshes the health collider and
selects directional sequence 0. At state-frame >= slot index +2 it selects state
1, sets enter and resets the frame counter; the unconditional final increment
leaves the counter at one on that transition.

## Matching evidence

Reviewed exact extents: 080427FC 320; 0804293C 72; 08042B28 40; 08042B50 52;
08042B84 88; 08042BDC 380; 08042D58 192. Each has a separate one-unit matching
commit and passed a full resolved-byte check. Hint minimization: health callback
15→3; directional sequence 7→4; display update 32→15; advance 15→2. Resource
contact and hiding need no hints. Existing typed consumers/destructor were also
checked. The display's sine scaling uses multiplication by 32, which preserves
the exact original instructions without a signed left shift.

The initializer's retained C is **380 bytes versus the reviewed 384-byte
original**, 88.6% fuzzy. High-register/initial-zero/CpuSet experiments worsened
it to 73–75% and 388 bytes; they were rejected. A separate tracking attempt is
77.7%, 308 versus 300 bytes: mask and source-position handling are recovered,
but timer/mode control flow and allocation differ. Candidate checkpoints are
under build/candidates/c22371f5b3c16f3b and build/candidates/4f0abb8083e711c2.
No draft or checkpoint earns exact credit.

## Upstream integration and next subsection

Claude's PRs 85–90 are incorporated: the decoded-data manifest/actor registry,
archive and sprite readers, compiled-script decoder, engine/command-name docs,
and thirteen additional unpinned exact functions. `ScriptProc_Start` is now
understood to be a **resource lookup by type/name hashes**; its existing symbol
is retained to avoid concurrent rename churn. The registry roundtrip check prints
`actor_registry: OK (0x1698 bytes, keyed_callbacks)`. Those tooling/documentation
and unpinning changes add no exact code percentage.

The complete combined build printed **`build/boktai3.gba: OK`**
(`build/contact-actor-final-combined.log`), including the new data generator.
Format, inventory, RAM-literal and decoded-data checks passed. Next contiguous
work begins at **08042E18**: target
tracking, the larger travel/update state at 08042F44, and subsequent handlers.
Tracking periodically checks squared distance against 0x22E24, retargets from the
manager mode/timer, and selects states 1/4/6 on observed conditions; its source
state/dispatch mapping still needs review. Do not repeat the logged constructor
or tracking variants without a new hypothesis.
