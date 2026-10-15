# Actor behavior and collision setup (0807462C–08076034)

This contiguous cohort continues the actor profile/registry review ending at
`7462C`. The natural behavior boundary is `76020`, after `75FC8` crosses the
initial `76000` assignment boundary. The adjacent 20-byte `76020` setter is
included as API glue; profile-driven animation selection starts at `76034`
and belongs to the adjacent worker. The cohort contains 15 entries and
6,664 reviewed bytes. No runtime coverage is inferred from static review.

## Receiver registration and object/context glue

`7462C` skips registration when scene flags `030053F4 & 4800` are set. Otherwise
categories `{4,5,7,6,8,21,25,23,18}` register the receiver at actor+C with the
actor's ID and receiver kind3, then set `flags2C0 & 100`. The category sets and
numeric receiver kind describe the observed contract; they do not establish
specific game-character identities.

`76020(actor,context,kind,object)` writes context3D0, object0, and the low byte
of kind to drawableKind8. It does not write drawable4. This was prior exact C;
its typed review receives no additional matching credit.

## Collision property views

The first collider is embedded at actor+134, the second at +188; their spacing
matches the shared `ContactCollider` size54. Names such as Body/Attack are
provisional interpretations of those two setup groups and their consumers.
All six helpers initialize using the low16 actor ID, argument3=0, argument4=16,
and caller-supplied size/offset pointers. They then set position to actor+5C
and attach the supplied callback and owner through the generic collider API.

The actual engine setters establish the property destinations:

| Actor source | Width | Collider destination | Meaning retained |
|---|---:|---|---|
| 9E | byte | halfword3C | field3C, not the separate cost field3E |
| A2 | halfword | word38 | reactionFlags |
| A4 | halfword | word34 | field34, otherwise unconfirmed |
| B4 + index×2 | halfword | halfword3C | indexed field3C |
| BC + index×2 | halfword | halfword40 | indexed field40 |
| C4 + index×2 | halfword | halfword44 | indexed field44 |
| CC + index×4 | word | word38 | indexed reactionFlags |

`scene_actor_collision.h` records the four-slot layout suggested by the array
spacing. The indexed read helpers use separate GNU trailing-storage views,
so their C expressions do not introduce a four-entry array bound. The selected
entries must lie in valid, initialized storage in the actual actor object.
The setup functions retain the caller's full word index and add no clamp.

An initial naming draft confused actor9E with the collider cost. Source audit
corrected it: `0821FF7C` writes its inputs to 3C,38,34, while `ContactCollider`
places the separate cost halfword at3E. The renamed helpers and all affected
bodies were rechecked against the ROM; this naming correction adds no credit.

## Exact setup contracts

| Address | Bytes | Behavior | Credit |
|---|---:|---|---|
| 0807462C | 108 | Register selected category receivers | New, pinned |
| 08074BF4 | 112 | Embedded collider134, flags5109, direct properties, refresh | New, ordinary |
| 08074C64 | 112 | Same, caller flags narrowed to16 bits | New, ordinary |
| 08074CD4 | 96 | Caller collider, narrowed caller flags, direct properties, refresh | New, ordinary |
| 08074D34 | 140 | Embedded collider188, flags2401, indexed properties, field34=0 | New, ordinary |
| 08074DC0 | 120 | Caller collider, flags2001, indexed properties, field34=0 | New, ordinary |
| 08074E38 | 120 | Same, caller-selected field34 | New, ordinary |
| 08076020 | 20 | Set object/context/type | Prior exact, pinned |

The last three helpers retain their observed lack of `Collider_Refresh`.
The current frozen subset adds **+0.0335 percentage points** (808 bytes):
700 ordinary and108 register-pinned. The prior exact setter contributes20
reviewed bytes without new credit. Each body passed a full-size fast check;
the parent performs promotion and one complete combined ROM build.

Receiver registration reduced5→2 hints, the old setter3→2, and the first
collider initializer3→0. Small ordinary inline property readers reproduced
the original address/read ordering where register barriers caused worse global
allocation. The indexed embedded initializer reduced2→0 hints. No handwritten
instruction bodies or explicit r7 bindings are used.

## Remaining contiguous bodies

The remaining seven functions retain assembly until their full spans match:
route-entry transition74698 (288), movement helpers747B8 (736) and74A98 (348),
movement update74EB0 (1,624), forward probe75508 (128), actor update75588 (2,624),
and availability callback75FC8 (88). Their behavior names are tentative review
labels, not promoted complete contracts. The prior registry-wide state request
70910 remains a separate 304-byte gap; its bounded permuter found no saved
improvement over the valid three-byte scratch-alias candidate.
