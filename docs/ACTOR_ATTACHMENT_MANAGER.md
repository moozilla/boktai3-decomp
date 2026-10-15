# Attachment manager lifecycle and scene actor pool prefix

This review covers `08065C40–08067148`. The preceding attachment slot update
cohort is documented separately by the adjacent worker. Names are tentative:
these records attach sprites or three part sprites to another actor, but the
screen or story identity has not been established from observed execution.

## Manager and slot ownership

`02000124` points to a task allocated in category 9 with size `A20`. Its two
animation-data records begin at `18` and `34`; resources `363C`/`363D` populate
those records. It caches part resource `1C1C` at `58`, and resource lookups
`(922E,6830)`/`(922E,D41F)` at `50`/`54`. These are resource hash/context pairs,
not evidence that the manager starts script processes. Eight `138`-byte slots
begin at `5C`, followed by the occupied mask at `A1C`.

The slot allocator forwards the manager to `65BEC`. Release helpers require a
manager, valid signed slot index, and matching callback identity. `65C50` also
requires the requested resource kind; `65D1C` handles the part resource path.
Kinds 0/1/4/5/6 remove the sprite only when byte `48` is set. Other kinds remove
up to three `28`-byte part records according to the byte mask at `3C`. Successful
release clears all `138` bytes, then writes `FF` to the signed index at `3D`.
Byte `31` drives manager dispatch and availability; byte `48` is specifically
sprite presence. These are distinct from the three status bytes at `36`.

The manager tick walks all eight slot states through table `08605B34` when its
occupied mask is nonzero. Destruction removes active part records and any
present ordinary sprites before clearing the singleton. Resource initialization,
singleton creation/reset, and the inverted availability query now have typed
contracts and names, preserving their previously exact bytes.

`65DBC` creates an ordinary sprite slot. It records an anchor, callback, source,
and follow descriptor; initializes a ten-frame timer and first status byte;
and chooses resource kind 6 when game-state flag `868 & 80` is set, otherwise
kind 0. It selects graphics A for kind 0, graphics B for kinds 1/4/5, and no
graphics for other kinds. Action `FF` copies the follow actor's offset at `34`;
other actions derive an offset from resource kind, action, and facing quadrant.
The follow descriptor's direction byte is at `9`, its anchor at `C`, and its
packed offset at `34`. The sixth source argument to `64C48` is forwarded but
unused by that geometry helper. Sequence setup selects manager entry `50` for
kind 0 and `54` for kinds 2/3. Sprite position is copied from the anchor as a
complete packed coordinate record.

## Recovered release-all boundary

`66CB8–66DB8` is a complete independent 256-byte function after the 44-byte
`66C8C` query. Its own push/save prologue, occupied-slot loop, sprite/part
cleanup, and pop/bx return do not belong to that preceding query. A search of
the entire ROM found no stored Thumb pointer `08066CB9`, and no halfword-aligned
Thumb direct B, conditional B, or BL encoding targeting `08066CB8`, including
encodings inside data. It is therefore tentatively named `ReleaseAll`, with no
execution or caller coverage claimed. Its manager guard and nonzero-mask guard
precede an eight-slot loop; unlike ordinary release, it does not filter callback
identity or resource kind before selecting the appropriate removal path.

Both extra-boundary CSVs and the progress inventory now record this entry. The
function denominator increases by one; the code-byte denominator is unchanged.
These 256 bytes were previously inside an already-C inventory span, so their
explicit exact C receives no new net byte credit. The previous query remains a
44-byte exact unit. No generated assembly is committed.

## Following scene actor control pool

The next subsection begins at `66F90` and uses singleton `02000128`. The shared
`scene_actor_control.h` belongs to the adjacent worker's continuation: eight
`AC`-byte slots begin at `3C`, after an occupied mask at `38`. Slot kind/state is
at `3E`, flags at `40`, signed index at `42`, and jitter timer at `44`. A contact
collider begins at `50`, with a saved position at `A4`.

Departure requests and collider contacts set slot flag 2 and collider flag 4.
Jitter requests set flag 4. Both indexed requests reject negative slot indices
and slots whose signed stored index is negative or whose kind/state is 2.
`67018` resets an expired jitter timer to 20 and clears request flag 4. During
the first half of a positive timer it perturbs X and Z by independent random
values modulo 40 minus 20, then decrements the timer. This continuation is
shared with the adjacent worker; names describe observed effects and do not
identify the surrounding scene.

## Exact accounting and remaining gaps

New ordinary-boundary exact bodies in this batch are `65C50` (204), `65D1C`
(160), `65DBC` (532), `66AE0` (72), `66B28` (144), and `67018` (136): **1,248
bytes, +0.0517 percentage points**. All currently carry temporary register-pin
concessions, reduced with `tools/depin.py`; none binds `r7` or embeds instruction
bodies. `66CB8` adds explicit correctness evidence for previously credited bytes.
Every changed C unit is checked across its full reviewed inventory extent.
The orchestrator performs the combined ROM build before integration.

Unmatched bodies remain `65FD0` (680), `66278` (508), `66474` (612), `666D8`
(472), `668B0` (472), and `66DB8` (472). The first three form the value/part
creation family. Two exact forwarding wrappers at `66A88`/`66AB4` expose its
fixed-kind and explicit-kind call contracts. The descriptor dispatcher at
`66DB8` accepts kinds 0–5; frame `7F` requests release, while other frames first
release all resource variants for the same identity, then create the requested
kind. Descriptor flag 4 selects action `FF`. Its useful 94.0% draft is private
and runs through the permuter; it earns no exact progress. The invalid-kind
path retains the original incidental return register, and callers reviewed for
this contract use the 0–5 domain.
