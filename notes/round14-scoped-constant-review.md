# Round 14: independent block-local constants

Tags: #round14 #concessions #matching #cleanup

This pass follows the conservative constant-removal audit. The first pass excluded
functions that declare `zero` or `one` in several independent blocks. Those names
can refer to distinct objects; a function-wide text replacement would be wrong.

The source baseline is `ea3b1c59d`. The bounded follow-up proposed 103 removals in
49 units after resolving explicit lexical scopes. **34 locals in 18 units were
removed**, with no replacement storage or source steering. All 12,108 bytes in
those units passed independent stock checks with the original complete extents.

## Accepted changes

| Unit | Locals removed | Integration commit |
|---|---:|---|
| `src/link/wireless/connection/sub_08035288.c` | 1 | `600782650` |
| `src/link/wireless/trade_menu/sub_080386C8.c` | 1 | `edf27dbda` |
| `src/link/wireless/trade_menu/sub_08038928.c` | 1 | `bdfcdbbc8` |
| `src/link/wireless/battle/sub_0803E998.c` | 2 | `9610d6c9f` |
| `src/link/wireless/battle/sub_0803ED84.c` | 1 | `8e4bd9025` |
| `src/effects/particles/proximity_emitter/sub_08051A8C.c` | 2 | `48b466289` |
| `src/objects/pile_driver/sub_0805AE58.c` | 2 | `b967776fb` |
| `src/scene/actor/registry/sub_080714E4.c` | 1 | `c0e5a4f91` |
| `src/scene/actor/reaction/sub_08080868.c` | 1 | `098c7c036` |
| `src/player/interaction/sub_08142CC0.c` | 1 | `e0f2d1b01` |
| `src/player/interaction/sub_08142FC4.c` | 1 | `e077ab8b9` |
| `src/player/attack/sub_0814B1F8.c` | 1 | `e7fe304db` |
| `src/bosses/hresvelgr/sub_0819C29C.c` | 2 | `638f4df4d` |
| `src/bosses/vanargand/sub_081A96E0.c` | 3 | `82d1bb19b` |
| `src/bosses/vanargand/sub_081A9910.c` | 4 | `b5059a10e` |
| `src/bosses/vanargand/sub_081A9DDC.c` | 2 | `fd86aef06` |
| `src/bosses/vanargand/sub_081A9F88.c` | 4 | `4cfa9f021` |
| `src/bosses/vanargand/sub_081EF914.c` | 4 | `eb78da36e` |

## Source and diagnostic checks

Each accepted diff was reviewed. Every use of the removed object becomes the same
signed or unsigned literal; `u32` values retain an unsigned literal. Other bindings
with the same name are untouched. The pass skips aggregate members, address-taken
fill objects, mixed-value assignments, unknown shadowing declarations, and unsafe
unbraced assignments. It introduces no aliases, casts, declaration-order changes,
loops, macros, or suppressions. No header or compiler setting changed.

The compiler-aware before/after scan over the 18 accepted units reports:

| Kind | Before | After |
|---|---:|---:|
| `const-local` | 47 | 13 |
| `local-union` | 1 | 1 |
| `proved-zero` | 1 | 1 |

Only `const-local` decreases, by 34; no other warning kind or site increases.

## Remaining work

[round14-scoped-constant-audit.tsv](round14-scoped-constant-audit.tsv) preserves
all 103 proposed bindings, original source locations, acceptance decisions, and
commit IDs. The 69 unselected proposals remain open. These results measure one
natural literal-removal hypothesis; they do not establish that the remaining
functions lack an ordinary exact form. Some need shared state operations or
better field types, and some named values are not constant storage at all.

The scratch baseline/candidate sources, fixed-hypothesis results, and complete
stock/linter checks are under `build/round14/scoped-const-pass/`. Accepted source
and the complete proposal ledger are preserved in this bundle.
