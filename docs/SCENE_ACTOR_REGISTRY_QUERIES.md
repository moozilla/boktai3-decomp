# Actor-registry queries (08070070–08070FA8)

This contiguous review follows the terrain/grid helpers and shares the narrow
`scene_actor_registry.h` view with the manager/lifecycle review beginning at
`08070FA8`. The singleton manager is `020004A8`; `020004AC` points to linked
nodes containing next pointer `0` and actor pointer `4`. The allocated terminal
node contains a NULL actor. These query helpers require an allocated head;
they do not guard an absent registry. Names describe observed contracts and do
not establish runtime coverage or specific game-character identities.

## Observed actor fields

The shared view identifies ID `50`, position `5C`, category byte `98`, signed
health `9C`, flags words `2CC/2D0`, flags halfwords `2D4/2D6`, and context `3D0`.
Byte `98` is provisionally a category: it selects stable sets of numeric values
in the reviewed queries. The larger movement setup (`70070`, still assembly)
uses behavior-state bytes around `2A8`–`2B6`, distinct from category `98`.
Halfword `1E6` is an additional equality filter in `70444`; the adjacent worker
also observes palette-related use of this region. Its complete meaning remains
unconfirmed, so the query does not name it as a particular actor subtype.

## Query contracts

`703A8` counts actors with positive health and no flag `2D0 & 20000`. `703F0`
adds exact equality with category `98`, using a full-width caller argument.
`70444` adds equality between halfword `1E6` and a second full-width caller
argument. `704A4` returns the first actor satisfying the same health/flags test.
All four retain an ignored call to `72B44`, which simply returns the manager
singleton. Removing that call would change the observed body.

`70EB0` returns the head node's actor. `70EBC` finds an actor with ID `50` equal
to the caller's word argument. The two script wrappers `70EE4` and `70F10` read
`Script_GetValue`, reject zero, and pass the value explicitly to this ID lookup.
The former tests positive health; the latter tests presence. The old exact C
incorrectly declared the lookup with no arguments and happened to preserve R0.
The corrected typed calls retain all original instructions.

The exact searches `70E00` and `70E44` use zero as a start
marker. A nonzero argument scans past the actor with that ID, then considers
subsequent nodes. Candidates require `flags2CC & 1` clear. The filtered variant
also requires category in `{4,1,3,5,7,6,8,25,21}`; the order of its comparisons
is preserved. The names' "available" interpretation is provisional: the bit's
complete lifecycle meaning has not yet been established.

`70F2C` traverses with `70EB0` then `70E00`, seeking its requested category and
any bit from the caller's `flags2CC` mask. Its availability predicate rejects
`flags2D4 & 40`, `flags2D0 & 20000`, and nonpositive health. The inline predicate
retains the observed NULL-pointer case and normalized zero/one result before the
caller checks its flag mask.

## Scripted control changes

`707EC` reads keyword `n` (actor ID), requires a non-NULL field5DC pointer, copies
signed fieldA8 to fieldAA, and sets `flags2D6 & 2`. The roles of the copied
fields and pointer remain unconfirmed. `70A40` reads `n` and `d`, requires the
actor's `flags2C0 & 100`, then calls movement-mode setup `70070(actor,d)`.
`70A90` applies the same setup to registry actors that pass the availability
predicate and have `flags2D4 & 4000`; it reads only keyword `d`.

`706DC` resets eligible categories `{4,1,3}` after the same health/status test.
It copies control5F4 to control21C, writes field2B2=1, field2B0=0, substate2B5=5,
clears field2C8 and field104, and clears bit one of flags114. It then resets
state2A8/phase2A9/counter2AC, chooses behavior2AA=3, sets field2B1=1, and sets
`flags2D0 & 1000`. These numeric states are deliberately retained: the reviewed
stores do not establish a game-level departure/reset animation identity.

## Boundaries and validation

The section spans 3,896 bytes and has 22 function entries, including known
independent entries `70444` and `70E44` that gbadisasm leaves as data. Thus
`703F0` has reviewed size 84, followed by the separate 96-byte `70444`; `70E00`
has size 68, followed by the separate 108-byte `70E44`. These entries already
exist in the repository's build/progress extra-function metadata.

The current frozen query/control cohort contains eleven new exact functions,
**+0.0492 percentage points** (1,188 bytes), all register-pinned. Four prior exact functions
(124 bytes) were reviewed and typed without additional credit. Each function
passed a full-size fast check after formatting. The parent performs name
promotion and one combined complete ROM build before integration.

| Address | Reviewed bytes | Contract | Credit |
|---|---:|---|---|
| 080703A8 | 72 | Count living actors | New, pinned |
| 080703F0 | 84 | Count living actors in a category | New, pinned |
| 08070444 | 96 | Count with category and field1E6 filter | New, pinned |
| 080704A4 | 72 | Find first living actor | New, pinned |
| 080706DC | 272 | Reset control/state for selected categories | New, pinned |
| 080707EC | 72 | Restore scripted actor fieldAA | New, pinned |
| 08070A40 | 80 | Set scripted actor movement mode | New, pinned |
| 08070A90 | 140 | Set movement mode across eligible registry actors | New, pinned |
| 08070E00 | 68 | Continue after ID, excluding flag bit one | New, pinned |
| 08070E44 | 108 | Continue with category whitelist | New, pinned |
| 08070EB0 | 12 | Get first actor | Prior exact, ordinary |
| 08070EBC | 40 | Find actor by ID | Prior exact, ordinary |
| 08070EE4 | 44 | Test scripted actor's health | Prior exact, ordinary |
| 08070F10 | 28 | Test scripted actor's presence | Prior exact, ordinary |
| 08070F2C | 124 | Test category and flags among eligible actors | New, pinned |

Register-hint minimization reduced the first four new leaves 5→3, 3→3, 11→7,
and 3→2 respectively. The continuation helpers retain 3 and 3 hints (the latter
reduced from 8), and the flags query reduced 17→8. No handwritten instruction bodies or explicit r7 bindings are used.
The control reset reduced 42→17 hints, the field-restoration script 7→3, the
individual movement-mode request 9→3, and the registry-wide mode request 21→10.

The control reset also demonstrated the compiler branch-length estimate issue:
its near-exact body measured 276 bytes, with a long entry branch and consequent
pool padding. Removing one barrier at a time only when the score improved found
no solution. A greedy pass accepting an unchanged fuzzy score removed ten empty
barriers; trial twelve shortened the compiler-selected branch and yielded all
272 bytes. The barriers emitted no physical instructions, but affected the
compiler's estimated branch distance. Normal exact-only depin followed this
preliminary minimization.

The remaining section includes movement/response setup, script-driven actor
mutation, and additional selection/query helpers; those bodies retain assembly
until their complete reviewed spans match.
