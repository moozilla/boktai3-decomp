# Ghoul contact response: 0808B3EC

Tags: #matching #ghoul #scene-actor #regalloc #0808B3EC

`Ghoul_ReactToContact` (formerly `sub_0808B3EC`) is a complete ordinary-C
match: **100% of its 6,236 bytes**. There are no register pins, assembly barriers, handwritten
instructions, or compiler overrides.

## What the callback does

The callback consumes contact flags, selects the hurt animation, and recovers
pursuit through the same arrival, terrain, random-turn, and control operations as
the matched Skeleton/Golem family. The Ghoul-specific paths use sounds `0x211` or
`0xD4`, tick the delayed script, clear the low halfword of context flag `0x628`,
and set the animation timer/latch to 45/65. Its first successful recovery sets a
32-frame timer; the normal pursuit success uses `Div(64, 3)`. Unlike Skeleton,
it does not clear the body pursuit timer during each partner-route retry.

## What decided the match

The implementation reuses the canonical actor and chase records and the shared
chase headers. A scalar coordinate-to-cell adapter, byte-domain state selection,
direct fractional coordinate reads, and separate route-failure clears recover
the target's 45 field-address stack slots. These are measured source-interface
choices, not proof of the original authors' exact helper names or boundaries.

The last two mismatches were caused by manually sharing all failed-route exits
with a `goto`. Writing each real callback-and-return at its own route failure,
as in the exact Skeleton sibling, lets GCC perform the sharing later. The
same-byte compiler trace contains six `Callback_InvokeActor` calls through
global allocation/reload and four after `jump2`: two outer copies disappear
only after they have affected reload-register rotation. This fixes both outer
register sequences and the four-byte size shortage without any source padding
or diagnostic knob. The final trace has 2,515 pre-PRE instructions and 1,257
hash buckets; all 45 field-address slots and all machine instructions match.

## Evidence and integration

The isolated ordinary compile, uninstrumented stock compiler, and complete
translation-unit production pipeline all produce the exact ROM slice at
`0808B3EC`. Preprocessing, compilation, and assembly are warning-free. This unit
contains one emitted function. The descriptive name is promoted in the symbol
registry and its caller. All shared-header consumers remain exact after the
common interface extraction; the batch integration owns the final combined ROM
build.

Code SHA-256:
`a2f2b3f9e09195dfff627fb2b81c23e6029cce8c2e188bef6539977bcc2c9c17`

Integrated source SHA-256:
`367a336d479f93e00e49fc45624a5afb9079f8c468772c2ec57c71ce9ba1fcf2`

The 25-node portable source history, rejected controls, stock gate, and complete
compiler traces are retained under `build/round4/ghoul/`; the compact entry is
`exact-summary.json`, the complete-unit gate is `full-tu-exact/verification.json`,
and the checked database backup is `checkpoint-exact-final.sqlite3`. The shared
adapter/provider extraction is complete in `scene_actor_chase_reaction.h` and
its implementation headers. No copied Ghoul helper layer remains.
