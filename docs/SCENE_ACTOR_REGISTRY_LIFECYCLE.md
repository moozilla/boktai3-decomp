# Registry list, message dispatch and update cadence (tentative)

The entire adjoining `0806FAD4–08070070` span is exact:1,436 bytes, including
1,036 newly recovered bytes and400 previously matched bytes with shared types.
The registry owns a terminal eight-byte node with null next/actor fields; active
nodes prepend to this list. Its manager retains the sentinel at1C and a rotating
cursor at20. The shared header also exposes queued message records at18 and
the actor's indirect callback at24C. These are static contracts; no game-object
identity or runtime execution coverage is inferred.

## Registration and removal

Registration requires a manager, fewer than twenty active actors, and a unique
word actor ID. With game-state flag868&4 set, the pair-record capacity query must
also permit the current030039F0 halfword and the actor ID narrowed to16 bits.
A successful eight-byte allocation is cleared, linked and published. Category1
sets game-state flag868&1. Unless actor flag2D0&02000000 is set, registration
queues terrain-overlay command0 using the actor ID address. Finally it ensures
category-dependent subsystems and increments both the active count32 and the
registration counter62. Allocation success is an original precondition; the
routine adds no failure check absent from the original.

Unlinking removes the first node with the given actor pointer, updates the cursor
to its successor, frees that node and decrements only count32. Removing the head
also changes the global head. Counter62 is not decremented here. The terminal
sentinel remains owned by the manager destructor. On alternate manager frames,
the cursor helper marks actor byteEB bit2 and advances, or wraps to the global
head when its cursor/actor is absent.

## Message filters

Message records are eight bytes: receiver ID at0, an unknown halfword at2 and
a payload pointer at4. The dispatcher queries records for manager resource24
and reads a16-bit opcode from each payload. Opcode1 invokes broadcast dispatch;
opcode2 filters by byte category; opcode3 filters by category and a16-bit actor
ID, stopping at the first matching actor with a callback. The broadcast helper
acts only when its selector byte is zero. An adjoining helper additionally
filters category and actor palette offset1E6 against byte arguments. All filters
skip null callbacks. The08249240 helper is a `bx r2` trampoline: it invokes the
callback with actor and payload arguments. Traversal excludes the sentinel.

## Cadence and subsystem creation

The availability count excludes actors with flags2D4&40, flags2D0&20000,
nonpositive health, or flags2CC&3. Given N eligible actors, the scheduling helper
uses quota1 for N≤4, quota2 for5–9, and quota4 above9. It computes
`ceil(N/quota)-1` as the delay. Ineligible actors have countdown byte3CC cleared;
nonzero eligible countdowns decrement. At most quota eligible zero-countdown
actors receive the new delay and flag2D6&4000 per call. This describes request
scheduling, not a proven frame-rate or gameplay identity.

Category2 lazily ensures the scene actor-control task (flag8), and palette
offset1 additionally ensures the task created at08121B40 (flag10). Other
categories ensure the attachment manager (flag20). Category3 also ensures the
particle-burst singleton; category14 ensures the task created at08122610
(flag1000). Successful creation stores both the task pointer and its ID from
Task+10. A null creation result returns immediately, retaining prior successful
subsystem setup. The two unnamed creator identities remain provisional.

The new functions retain initialized register/memory lifetime barriers where
needed. Depinning leaves one barrier in counting, one in registration, one in
unlinking, one in category/palette dispatch, one in cursor advance, ten hints in
cadence scheduling and four in subsystem setup. No instruction bodies or
undefined register tokens are used. Every function is checked over its full
reviewed body, pool and padding extent before integration.
