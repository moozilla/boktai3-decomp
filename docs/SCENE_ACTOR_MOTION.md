# Registered actor motion phase (tentative)

Reviewed ownership: `0807A8CC–0807C11C`. The two full entries are 1,736 and
4,488 bytes, including their pools. Each has one shared return epilogue; no
hidden function boundary has been established inside either entry.

## Update callback, 0807A8CC

The callback marks byte `2B4` enabled and advances either the sequence-backed
sprite (`drawableKind == 0`) or the tile animation. Byte `2B3` records animation
completion here. Availability rejects flag `2D4 & 0x40`, flag `2D0 & 0x20000`,
or nonpositive signed health at `9C`. All return paths produce 1.

The motion core receives the actor prefix at `50`. Its temporary map pointer
at `6C` is actor `40` only when flag `2CC & 0x2000` is present and flag
`2C0 & 0x20` is absent. The callback records the prior cell at `2F6`, recomputes
cell `2F4`, and uses flag `2D6 & 0x2000` to gate terrain processing. That flag
is set for nonzero X/Z velocity or equal previous/current X/Z coordinates;
the equality test is intentional evidence, not an inferred correction.

Except for categories 17–20 and 23, a changed cell can trigger two reactions:

- Cell flag `0x40` and current Y plus signed offset `2EA` at or below unsigned
  height `2BE`: install callback `238`, falling back to `08081290`; use kind22;
  clear the transient control fields; enter state3/phase0/behavior0; set the
  reset byte and flag `2D0 & 0x20000`.
- Cell flag `0x80` plus game flag `868 & 0x10`: form the cell center and the
  difference from actor position; save the quantized angle at `2B6`; write
  payload32, signed-health-derived halfwords in the primary collider and `2E4`,
  collider halfword40, parameter512, mode1, and flag `2CC & 0x2000`.

For category3, odd movement ticks and flag `2D4 & 0x100` queue flag
`2D0 & 0x10000`. A fresh animation frame consumes that flag through the nearby
repeated-effect request. This is an effect trigger; a specific footstep or
screen identity is not established.

The common tail can update contact object `5D0` for flag `2CC & 0x400000`,
increments the halfword tick counter `2CA`, updates the embedded control group,
copies the full eight-byte position to `2FC`, and clears motion flag `0x2000`.
The center and difference are stack position objects. Only initialized XYZ
members are used as scalar values; their padding is not read as an integer.

## Adjoining route callback, 0807AF94 (review in progress)

Initial gates are group flags at `414`: mask `0x2` suppresses the callback, mask `0x1`
permits it. The original then dereferences actor `DC` and compares actor Y to
that object's Y at `0E`. The object must exist on this path; no invented null
fallback is justified.

Unequal height updates destination `5D4` from route point `450` (X/Z), queries
terrain height, and selects a player-backed or object-backed cell according to
global flags. Equal height quantizes the heading and tests adjacent cardinal
cells with repeated `0812E118` traversals. It then scans equal-height cells
until flags `0x43` or a height change delimit a destination. The control-save
records at `460` and the direction/countdown bytes at `5D8–5DA` are phase
storage; their broader ownership remains tentative.

Neither a fuzzy candidate nor an assembly fallback receives exact credit.

## Private matching checkpoints

Neither full entry currently receives exact credit. `build/actor-A8CC-v74.c`
is the complete 1,736-byte callback at 99.3%, seven differing bytes: two
terrain-width LDRSH scratch pairs and the second health LDRSH scratch pair.
`build/actor-A8CC-v51.c` is the same compiled baseline before parser-only
cleanup. `build/actor-A8CC-v56.c` is an alternate 99.3% checkpoint with both
width scratch pairs exact but three health scratch pairs differing.

The route callback has a complete control-flow draft; further pointer-cache
review and allocation work remain. Its large frame comes from
control/state field pointers initialized before the RNG retry loop and kept
live across it, twice in the entry. Four separate traversal-count slots occupy
`SP+24/28/2C/30`; direction and saved heading occupy `SP+1C/20`. The explicit
cache in v9 and successors produces the reviewed `0xB4` frame. v8 scores
53.2% at 4,540 bytes; later checkpoints preserve a destination pointer across
the terrain query, matching the original caching behavior. The first height
path reuses destination X/Z halfword values before querying the cell, rather
than reloading them from memory.

The upstream randomizer rejected a cache declaration with `center, difference`
in one struct-member declaration: pycparser shared the same Struct type node,
violating compute_node_indices' one-node/one-parent assertion. Separate member
declarations eliminate that duplicate without changing bytes. The next
randomizer assertion rejected the function-local standalone `struct TileState`
definition; moving that type definition to file scope also preserves all
compiled bytes. These are private source/parser workarounds, not shared tool
changes. The refreshed v74 search log is `build/actor-A8CC-v74-permute.log`, run
`build/permute/sub_0807A8CC/run-da5fqpph` (one job, one-hour cap). It starts at
permuter score30 and has passed the two prior parser assertions. Any improved
output still requires source review, full-extent checking and hint minimization.
