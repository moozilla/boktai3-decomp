# Production matching round 1 — Sol

Assigned range: `08170000 <= addr < 0822F248`. Baseline: `540daab`.

Matched 25 functions: 1980 emitted bytes, 1980 progress-span bytes. Fifteen manual/template adaptations and ten automatic family ports. 251 checks across 54 targets; 29 remained unmatched. No target exceeded ten checks. Active matching interval: 578.9 seconds (includes interleaved full-build verification).

Full ROM builds were run before each batch of at most five per-function commits; all six printed `build/boktai3.gba: OK`. Individual checks and build transcripts are in private `build/round1/`; previous `build/benchmark/` remains intact.

## Patterns

- Stack vectors: `struct V {s16 x,y,z;} v; s16 *q=(s16 *)&v;` with separate source-value temporaries and `v.x=a; q[1]=b; q[2]=z;` reproduced the first-store pointer copy that a plain array omitted (`081C08F4`, sibling `081C0958`).
- For multiargument calls, calculate the first argument pointer before calculating the second to match instruction order (`08170C08`).
- Explicit shared labels for found/overflow returns reproduced a cache insertion function’s original branch layout (`082172E0`); a common result variable added a copy and moved the pool.
- Clones with changed shift-factor immediates often fail automatic numeric substitutions because the source stores the combined offset. Manual template adaptation fixed `081BC980`, `081A53E8`, and the two one-store leaves.

## Preserved failures

- `081A55D4`: `wip/round1/sub_081A55D4-best.c`; only two loop-setup instructions at `081A55E8`/`081A55EA` are reordered. `build/round1/check-243.txt`. Nine checks total.
- `081FEDA0`: retained WIP; register lifetimes and stack-source/destination setup differ.
- `0818E1BC`: materialized boolean present, but pointer/register allocation adds an `ip` transfer. Retained WIP.
- `081C674C`: C callback call compiles to `_call_via_r4`, unresolved in check.py; original thunk is `sub_08249248`. Retained WIP, no symbol/tool workaround applied.
- Remaining automated template mismatches are preserved in `wip/round1/`; complete attempts/statuses in `build/round1/results.json`.

## Matches

`sub_08170C08`, `sub_081713A4`, `sub_08171410`, `sub_081735C8`, `sub_081780FC`, `sub_08178140`, `sub_0817AA5C`, `sub_0817AFE8`, `sub_0817B4B4`, `sub_0817B504`, `sub_0817B554`, `sub_081A5394`, `sub_081A53E8`, `sub_081BB7B4`, `sub_081BC980`, `sub_081BDB28`, `sub_081BECA4`, `sub_081BEDFC`, `sub_081C08F4`, `sub_081C0958`, `sub_081D1D44`, `sub_081D1D80`, `sub_081D1DBC`, `sub_082172E0`, `sub_08217344`.

## Compiler thunk follow-up

The ROM functions at `08249238`, `0824923C`, `08249240`, `08249244`, `08249248`, and `0824924C` are respectively `bx r0` through `bx r5`, each followed by `nop`. The agbcc build's `libgcc/Makefile` compiles `lib1thumb.asm`, whose `call_via` macro defines `_call_via_<register>` as precisely `bx <register>; nop`; it instantiates `call_via r4` after r0–r3. Thus `_call_via_r4 = sub_08249248` is an evidence-backed compiler symbol alias. Only the alias needed by the retained candidate was added.

Alias-only full ROM verification is logged in `build/round1/fullbuild-thunk-alias.txt`.

With the alias available, `081C674C` differed only in the order of two zero loads. Replacing byte-pointer casts with a struct containing a `u8` field at offset 7, a `u16` field at offset `0xE`, a `u32` counter at `0x10`, and the callback at `0x2CC` reproduced the original ordering. The callback's third argument is the pre-increment counter (original keeps it in r2). This follow-up matched 52 bytes; its full ROM verification is logged in `build/round1/fullbuild-thunk-match.txt`.
