# Mummy movement response: 080DA97C

Tags: #matching #mummy #scene-actor #regalloc #080DA97C

`Mummy_SelectMovementResponse` matches **all 6,316 bytes** in ordinary C, with
no register pins, assembly barriers, handwritten instructions, or compiler
overrides. It uses the shared pursuit/contact implementation rather than a
Mummy copy of the Golem or Skeleton helpers.

## Behavior and a draft correction

The response plays its variant sound, clears a collision flag, and prioritizes
route selection, knockback contact, busy/blocked motion, a special reaction,
and ordinary pursuit. The route-choice branch advances the random cursor,
avoids choosing the same route point when possible, resolves that position,
and installs behavior 8. Contact and pursuit reuse the shared arrival and
partner-position retry operations; successful pursuit installs behavior 7 and
the `Div(64, 3)` delay. Returning home uses behavior 12 instead.

The special reaction loads its callback from actor offset `0x230`. The rough
draft used `0x220`; the matched source uses the existing `reactionTarget230`
field. The group route-choice callback at `+0x214` and route selection records
are now named in the shared chase layout.

## What decided the match

First, route sampling needs the project's checked `RANDOM_TABLE_ADDR` form.
The integer address is materialized by reload, whereas the symbolic table
address becomes an allocated pseudo. That one additional reload explains the
otherwise shifted registers in the first state/control transition. The existing
RAM-literal build check ties the macro to `gRandomTable` in the link map.

Second, both successful and failed responses must be expressed at their own
outer branches. GCC later shares their identical tails. Hand-sharing them with
`goto failed` and `goto pursue` prematurely removed allocation history. Restoring
the source-local responses recovered every non-stack instruction exactly.

The remaining 45 address slots were solved with the validated PRE order model.
The selected common interfaces use a typed group receiver, arithmetic-word map
width, and direct route fields. The model predicted two different ordinary-C
compositions; both compiled to the exact ROM bytes. The simpler direct-field
version was retained. Its measured pre-PRE instruction count is 2,555 and its
table has 1,277 buckets. These facts explain the compiler outcome; they are not
source padding or constants embedded in the implementation.

## Evidence and limits

The ordinary evaluator, stock uninstrumented compiler, and complete translation
unit agree. The consolidated and formatted source compiles without warnings and
has zero differing bytes. Contact animation and player selection also remain
exact after sharing their existing Skeleton/Ghoul implementations.

Code SHA-256:
`437b5d9d7e4e7b074bf58c53f5aef6f84ed2962217f4e215459e8a4ab19e2f8b`

The portable source history, conditional composition model, strict slot report,
and full-unit gate are retained in `build/round4/mummy_select/`. There are 94
recorded source nodes. The exact output establishes behavior at the machine-code
level; the selected C interfaces remain non-unique reconstructions. The batch
integration owns the single final combined ROM build.
