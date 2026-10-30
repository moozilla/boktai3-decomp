# Round 7: Sword task initialization

Tags: #matching #monsters #sword #080F67AC #scope

`Sword_InitTask` at `080F67AC` is an ordinary-C match over its complete
2,020-byte body and literal pool. It initializes the main drawable, shadow,
colliders, palette and four child sprites. Function metadata and the existing
`Sword_InitializeFromScript` call now use the descriptive name.

The match retains typed sequence and collider records. A palette-entry
indirection gives the palette copy its actual halfword alias type. Child sprite
setup retains its byte offset across initialization, then reconstructs the
sprite pointer. Both child geometry arguments are concrete values; the shared
contact flags are borrowed from a real value outside the child loop.

The inherited draft passed zero as the shadow initializer's extra size
argument. The ROM passes 48; the matched source corrects that argument.

The final playback mismatch was a comparison against a widened duration copy
instead of the raw duration. Explicit word casts still allowed the frontend to
shorten the comparison. A native halfword scalar also widened too early.
Retaining the elapsed snapshot in halfword array storage until its later read
keeps the raw duration load between the counter update and the widening. The
word duration and halfword hold value then have distinct, real roles. A byte
frame-count value, an owned flags read, and word hold-mask values reproduce the
original register ownership without pins, barriers or compiler overrides.

Validation: the stock per-unit checker reported zero differing bytes over
2,020 bytes before symbol promotion; stock scope also reported a complete
2,020-byte match after formatting and naming the production source. The batch
integrator performs the combined ROM and renamed-caller checks. The alternate
word-returning hold-mask helper also matches, and both accepted experiments
and the earlier independent frontiers remain in the recovery checkpoint.
