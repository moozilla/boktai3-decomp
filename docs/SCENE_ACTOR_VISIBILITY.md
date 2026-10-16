# Scene actor visibility and stored motion (tentative)

Reviewed contiguous range **08084280–08084F44 exclusive**, 3268 bytes.
This continues the actor3D0 control-context family. The name describes observed
visibility and stored-position movement; it does not identify one universal
actor species. Every entry and literal pool was reviewed through its next boundary.

| Start | End exclusive | Bytes | Observed behavior | Status |
|---|---|---:|---|---|
| 08084280 | 08084284 | 4 | Returns word 1 | Existing exact |
| 08084284 | 080844B8 | 564 | Approach context3FC; choose nearby player position when arrival helper accepts | ASM |
| 080844B8 | 0808483C | 900 | Random nearby-player target, approach, wait/restart, and collision response | ASM |
| 0808483C | 0808484C | 16 | Install ROM08605C5C at actor28C | Existing exact |
| 0808484C | 080849DC | 400 | Tick visibility transition and shadow state | New exact, register-pinned |
| 080849DC | 08084A60 | 132 | Start stored-position curve and enter state2/phase0/behavior3 | New exact, register-pinned |
| 08084A60 | 08084BCC | 364 | Request show/hide transition from packet halfword2 | New exact, register-pinned |
| 08084BCC | 08084DB8 | 492 | Approach stored target and consume pending trigger on arrival | ASM |
| 08084DB8 | 08084E50 | 152 | Consume pending trigger on entry and increment counter | New exact, register-pinned |
| 08084E50 | 08084E60 | 16 | Install ROM08605C64 at actor29C | Existing exact |
| 08084E60 | 08084E94 | 52 | Tick hidden response, dispatch state callback, update control | Existing exact |
| 08084E94 | 08084EB0 | 28 | Link actorDC to player0+24, initialize, return 0 | Existing exact |
| 08084EB0 | 08084F44 | 148 | Drawable/shadow/collider/registry teardown, return 0 | Existing exact |

New exact total is **1048 bytes**, approximately **+0.0434 percentage points**.
All four new bodies retain register-pinning concessions after standard depin.
The 264 existing exact bytes add no progress. Three remaining ASM bodies total
1956 bytes. No hidden boundary or previously credited span was added here.

## Visibility contracts

The visible-bit operations address drawable flags at +0 for drawableKind0,
and +28 for other kinds (the embedded tile state begins +20). Bit1 in the
numeric sense `flags & 1` means hidden. Show clears it and enables the actor320
shadow; hide sets it and disables the shadow.

The common availability predicate returns 1 for a nonnull actor when flags2D4
has40, flags2D0 has20000, or signed health9C is nonpositive. The predicate itself
returns 0 for null, so neither whole callback promises null safety.

`TickVisibility` is void(actor). It requires a nonnull actor. On an available
actor it requires a readable actor3D0 context, and only continues if that
context's flags3D4 contain20 or40. Its halfword timer3EE uses bit2 through count19,
then bit1 through count29, to alternate sprite and shadow visibility. Afterwards
bit20 settles shown and is cleared; otherwise bit40 settles hidden and is cleared.
The timer increments modulo65536 after each active update. These context flags
are distinct from the actor's own word3D4.

`RequestVisibility` is void(actor, const u16 *packet). The context pointer is
loaded before the availability gate, requiring a nonnull actor even on rejected
requests. After availability and actor flags2D4&4000 pass, the packet must expose
halfword+2. Value0 requests show only when currently hidden and context20 clear;
value1 requests hide only when currently shown and context40 clear. Accepted
requests zero timer3EE, clear opposite/control bits (50 for show,30 for hide),
set20/40 respectively, and play song21E. Other values hide immediately. The
context must be readable/writable on the0/1 paths; the immediate-hide path does
not dereference it. Drawable and shadow storage must be valid when those paths
are reached. No caller-derived unrestricted packet type is claimed.

## Stored motion and entry contracts

`StartStoredCurve` is void(actor), requiring the actor and its control context.
It advances the global RNG index modulo1024 even though it does not consume a
table byte. It copies the full8-byte actor2FC position and passes it to
StartPositionCurve83E98. It writes state2/phase0/behavior3/counter0, briefly sets
entry byte2B1, clears context flags3D4 bit8, then clears2B1. The aggregate copy
preserves the fourth halfword; no uninitialized local component is introduced.

`ConsumePendingEntry` is void(actor). It consumes2B1 as a0/1 entry latch. On entry
it copies actor228 to target21C and resets control bytes2B2=1,2B0=0,2B5=1,
field2C8=0, flags114 bit1 clear, word104=0; then consumes one pending trigger
at actor+C and sets flags2C0 bit800. Counter2AC increments on every call. The
actor228 pointer is copied as an opaque value; this body does not invoke it.

The unmatched approach callbacks use callback260 through the BX-r1 trampoline
0824923C and consume its word return as movement speed. They truncate X/Z target
deltas to signed16 before the abs<=32 arrival gate. Arrival83C38 is tested as a
low-byte result. Random position choices use copied player coordinates and
packed-halfword updates; a future C draft must preserve defined aggregate
lifetimes and intentional signed truncation. These bodies remain ASM.

## Source and legacy QC

All empty-asm input/read-write operands in the four new matches have assigned
values before use. Output register declarations are assigned before any C read.
Clobber-only empty asm has no operands and contains no instructions. Exactness
was checked over each complete extent, then hints were minimized and the
formatted source checked again. Removing unnecessary barriers can also avoid
old GCC's estimated-branch-distance expansion; physical branch distances alone
do not explain that compiler decision.

Related84180 requires a nonnull actor, and a nonnull context on the path where
actor flags2D4 bit100 is clear: that path reads context flags3D4 bit2. It sets
statusEB bit8 before that gate. It is not a null-tolerant API.

Legacy84E94 declares gUnk_02000580 as a byte pointer despite the canonical player
pointer-array contract; its ROM behavior loads player slot0 then adds24. Its
integrator should use the canonical player declaration when typing that old TU.
Legacy84E60's callback declaration is word-address based and void-returning;
its dispatch ignores the returned value, while the movement consumers above
need the actual word result. No extra exact bytes are claimed for either review.
Teardown84EB0 tests actor flags2D0 mask02000000 (not20000000) before03C10.
