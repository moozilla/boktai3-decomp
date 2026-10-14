# Parameterized motion pool, 080454C0–08045A20

The gameplay identity is tentative; the observed implementation is a sixteen-slot
pool registered under hash 0x5B1B. Its task allocation is 0x13DC, slot stride 0x13C,
and copied parameter packet 0x44 bytes. Seven new exact functions add **0.043
percentage points** (1,028 bytes). Existing callback, hide, factory, free-slot
finder, packet initializer and setters gain names/types with no new credit.

| Function | Address | Exact body bytes | New credit |
|---|---|---:|---|
| FinishOnResourceContact | 080454C0 | 8 | Existing |
| InitializeSlot | 080454C8 | 136 | Yes |
| HideSlot | 08045550 | 36 | Existing |
| Update | 08045574 | 276 | Yes |
| Destroy | 08045688 | 64 | Yes |
| Initialize | 080456C8 | 60 | Yes |
| Create | 08045704 | 64 | Existing |
| FindFreeSlot | 08045744 | 52 | Existing |
| Spawn | 08045778 | 336 | Yes, no hints |
| InitializeParams and setters | 080458C8–08045984 | 188 total | Existing |
| AdjustHeading | 08045984 | 116 | Yes |
| UpdateUntilDeadline | 080459F8 | 40 | Yes |

## Layout and lifecycle

Slot frame/flags are +0/+4, velocity +10, parameters +18, graphics +5C, sprite
+78, collider +A4 and optional target position pointer +F8. Initialization clears
frame/velocity, loads graphics, registers sprite and contact callback, and configures
the resource collider. Parameter defaults are deadline 60, heading/speed zero,
contact values 0/0/10/20/20, bounds 8/32/8, offset zero, graphics hash 848F,
frame zero, tile -1 and UpdateUntilDeadline. First parameter halfword +0 is
provisionally firstFrame; this default callback does not read it.

Create allocates list 8 and installs manager/destroy callbacks. Initialize
registers the pool and initializes all slots. Destroy hides each active slot then
unregisters hash 5B1B. FindFreeSlot samples the active mask once and returns the
first clear slot/index, or null. Hide sets the sprite hidden flag, removes it
from drawing, and clears the bit.

Spawn looks up the registered pool. If absent, it creates the pool but returns -1
without spawning on that call. It also returns -1 when full. Otherwise it copies
the complete packet, clears frame, computes initial velocity, copies collider
bounds/offset, positions sprite/contact, loads graphics/frame and optional tile,
and finds the configured target when trackingFrames is nonzero. The target
pointer is stored only when lookup succeeds; a reused slot's old target is not
explicitly cleared. Slot flags are also retained. It sets the active bit and
returns zero.

The existing owner callback at 0802D218 configures a concrete example: deadline
240, tracking 180, turn step 1, speed 12, player ID from player+24, graphics hash
61F9/frame 29/tile 29F, and source position raised by 128. It faces opposite the
owner heading and supplies bounds 16 on all axes. This supports a homing effect
interpretation but does not establish a game-specific identity or new coverage.

## Update and observed heading rule

For each active slot, Update calls its packet callback with slot/index. Return 1
hides it and skips movement/contact/frame increment. Otherwise flag 1 requests
velocity recalculation from signed heading and speed using the sine table and
truncation toward zero at scale 4096. It adds the packed velocity to sprite
position. Flag 2 updates and refreshes the collider, then frame increments.
FinishOnResourceContact sets frame to the configured endFrame, so the default
callback ends it on the next manager check.

AdjustHeading runs only while frame < trackingFrames and target is nonnull.
It computes desired angle, wraps the difference into a signed byte, and compares
that difference with the signed turnSpeed byte. The original compares against
**the same threshold in both branches**, not ±turnSpeed: above it adds the raw
step, below it subtracts, equality selects the desired heading. This unusual
rule is preserved exactly. It sets flag 1. UpdateUntilDeadline calls it, sets
flag 2 and returns whether frame has reached endFrame.

## Matching evidence

Shared collider offset and bounds are now packed positions at +14/+1C;
coordinates +24 and flags +6 retain their offsets. All 34 existing matching
contact actor/swarm consumers passed checks using their previously validated
object extents. All nineteen new/typed motion units passed reviewed full extents.
No new emulator behavior claims are made.

The manager initially differed only in an expanded loop branch. Removing
redundant barriers restored the short branch; 42 hints reduced to ten after
matching/minimization. Slot initialization retains one barrier, heading adjusts
15 hints to two, and the deadline callback retains one register hint. Destroy,
Initialize and Spawn are entirely unpinned. No r7 bindings or instruction bodies
were introduced. Ignored build/motion-* and candidate checkpoints retain the
failed load-order and allocation hypotheses.

Next subsection begins at 08045A20: a five-animation projected display, with
world-to-screen coordinates and resource-backed initialization.
