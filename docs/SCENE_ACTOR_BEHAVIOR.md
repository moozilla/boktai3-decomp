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
| 08074698 | 288 | Advance route point, run optional script, select behavior | New, pinned |
| 08074A98 | 348 | Update terrain/player height and synchronize drawable position | New, pinned |
| 08074BF4 | 112 | Embedded collider134, flags5109, direct properties, refresh | New, ordinary |
| 08074C64 | 112 | Same, caller flags narrowed to16 bits | New, ordinary |
| 08074CD4 | 96 | Caller collider, narrowed caller flags, direct properties, refresh | New, ordinary |
| 08074D34 | 140 | Embedded collider188, flags2401, indexed properties, field34=0 | New, ordinary |
| 08074DC0 | 120 | Caller collider, flags2001, indexed properties, field34=0 | New, ordinary |
| 08074E38 | 120 | Same, caller-selected field34 | Prior exact, ordinary |
| 08075508 | 128 | Project a forward probe and snapshot prior position | New, pinned |
| 08075FC8 | 88 | Clear action field, test eligibility, request available action | New, pinned |
| 08076020 | 20 | Set object/context/type | Prior exact, pinned |

The last three helpers retain their observed lack of `Collider_Refresh`.
The current frozen subset adds **+0.0638 percentage points** (1,540 bytes):
580 ordinary and960 register-pinned. Twelve of fifteen entries are exact,
covering1,680 reviewed bytes. Prior exact E38 and the setter contribute
140 reviewed bytes without new credit. E38 also corrects the old raw formal
argument widths to the actual collision-engine setters, retaining its match. Each body passed a full-size fast check;
the parent performs promotion and one complete combined ROM build.

Receiver registration reduced5→2 hints, the old setter3→2, and the first
collider initializer3→0. Small ordinary inline property readers reproduced
the original address/read ordering where register barriers caused worse global
allocation. The indexed embedded initializer reduced2→0 hints. No handwritten
instruction bodies or explicit r7 bindings are used.

## Route-point transition

`74698` optionally calls the actor script selected by halfword1EC, passing three
words: the embedded cursor index, actor ID, and category. The embedded cursor
at448 supplies that script index independently of the cursor argument. The
shared script argument convention initializes count and values while leaving
the unused halfword unspecified; the source does not read that halfword.

`0821E618` advances the supplied cursor, wrapping its index by definition byte0,
clearing cursor byte3, and calculating its point from the map base plus10,
definition halfword2, and index×8. Points contain X/Z at0/2 and attributes at6,
which explains the earlier `70070` two-halfword target read through actor450.
`74698` uses the high attribute nibble to index table08610814 and narrows the
word return of proximity helper0821E8C8 to a byte. Proximity selects numeric
behavior2 and setting2C2=64; otherwise the table selects behavior1 or0. Each
branch clears state/phase/counter and marks field2B1=1; behavior1 also clears
flags2D0 bit20. Game-facing behavior meanings remain tentative.

The full 288-byte transition matches with six register hints after minimizing
sixteen. Its scalar lifetimes and independent cursor/owner contract were
audited after minimization.

## Height, projection, and eligibility contracts

`74A98` computes terrain height from cell2F4, preferring the override cell returned
by0821D64C and falling back to map cells+12+index×4. The low nibble supplies height
in units256; high nibble1 subtracts the position's low Z byte, and2 subtracts low X.
Category22 adjusts displacement2EA by the change in ground height2BE. Categories
17–20 and23 can instead use player slot0 Y unless flags2D4 bit4000 is set.
Unless flags2C0 bit20 preserves Y, actorY becomes ground height plus displacement.
The position is copied as an aggregate into sequence drawable+1C or tile drawable+40.
Full348 bytes match, with six hints retained after minimizing seventeen.

`75508` uses direction byte2B6 and separate word scale arguments. It shifts scales
left2, multiplies them by sine(angle+64) for X and sine(angle) for Z, and uses an
arithmetic right shift12 on the wrapped word products. Projected X/Y/Z are stored
at310/312/314, retaining actorY; the prior ContactPosition is copied to318. The full
128 bytes match with two hints after minimizing sixteen. Padding travels only
through the aggregate copy, without C scalar reads.

`75FC8` first clears word304 unconditionally, so callers must supply a valid actor
even though its subsequent inline eligibility predicate handles NULL. Flags2D4
bit40, flags2D0 bit20000, or health<=0 reject the action. Otherwise78700 is called
and the callback returns1; rejected requests return0. The full88 bytes match with
five hints after minimizing eleven. The game-facing meaning of78700 remains
unconfirmed, so the action name is provisional.

## Remaining contiguous bodies

The remaining three functions retain assembly until their full spans match:
impulse/bounce747B8 (736), movement update74EB0 (1,624), and actor update75588 (2,624). Their behavior names are tentative review
labels, not promoted complete contracts. The prior registry-wide state request
70910 remains a separate 304-byte gap; its bounded permuter found no saved
improvement over the valid three-byte scratch-alias candidate.
