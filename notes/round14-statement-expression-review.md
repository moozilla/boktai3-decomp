# Round 14: remaining statement expressions

Tags: #matching #cleanup #round14 #compiler

This bounded pass follows `round14-wrapper-cleanup.md`. Its baseline is the
worker synchronization commit `b0dd6a380`, which includes the combined source
at `3af1eae410b915dae0dbe7bbdaa90a9aadfe0a9b`. Sixteen units contained the
remaining 24 reported statement expressions.

## Landed source changes

Twelve translation units lose 18 statement-expression warnings. Each final
source unit was independently checked with the stock compiler: all 47,392
bytes match. No header or compiler configuration changed.

| Commit | Unit | Statement expressions removed | Exact bytes |
|---|---|---:|---:|
| `b5371dafb` | `background_controller.c` | 1 | 5,372 |
| `3b4cfdfe5` | `scene_catalog.c` | 1 | 2,664 |
| `5666ce868` | `vorn_main.c` | 1 | 25,404 |
| `63b2b86ed` | `bat_restore.c` | 2 | 9,548 |
| `e5697e888` | `fn/sub_0801EC4C.c` | 1 | 732 |
| `0447996bf` | `fn/sub_0801F5EC.c` | 1 | 840 |
| `90e79c2ef` | `fn/sub_08038AD4.c` | 3 | 520 |
| `c9aa85a5b` | `fn/sub_0810D688.c` | 1 | 216 |
| `806244d84` | `fn/sub_0812A2BC.c` | 2 | 348 |
| `0c31602ee` | `fn/sub_0820C4D4.c` | 3 | 1,200 |
| `9809fd8bd` | `fn/sub_08241350.c` | 1 | 436 |
| `fd179f563` | `fn/sub_0824AC74.c` | 1 | 112 |

The substantive changes are direct field reads and calculations, ordinary
navigation tests, and ordinary control flow for script arguments and input
conditions. The two scene-object constructors keep a named type or mode
read before the existing cell-field setup. Screamer now uses the shared
state fields for all four accesses that previously constructed byte offsets.
The walking animation uses `DisplaySequenceState.index` at its existing
offset rather than reconstructing an address.

### One source-backed ignore, counted separately

The libgcc product-result statement expression is removed. Its inner
`umul_ppmm` loop is retained with one new `concession-ok(once-loop)` annotation.
This is separate from the 18 source removals above.

The pinned `pret/agbcc` source proves that the loop was present in the
original library. The complete pre-change function body is token-identical
to the preprocessed original `__muldi3`. Removing the loop was tested and
changes bytes. The exact upstream commit, files, expansion verification,
failed natural candidates, and annotation are recorded in
[round14-libgcc-multiply-source.md](round14-libgcc-multiply-source.md).

## Linter accounting

These are counts for the sixteen reviewed units, using the same linter
and diagnostic compiler for both runs. They are not repository totals.

| Kind | Before | After | Change |
|---|---:|---:|---:|
| `stmt-expr` | 24 | 6 | 18 source removals |
| `once-loop` | 12 | 11 | 1 original-source annotation |
| `const-local` | 55 | 55 | unchanged |
| `addr-deref` | 8 | 8 | unchanged |
| `volatile-view` | 1 | 1 | unchanged |
| `local-union` | 3 | 3 | unchanged |
| `proved-zero` | 1 | 1 | unchanged |
| Errors | 0 | 0 | unchanged |
| All warnings | 104 | 85 | 18 removals and 1 annotation |

No warning kind increased. Existing header macros and unreported source
forms are not certified clean by this count.

## Six sites remain open

The four units below retain their original source and diagnostics.
[round14-statement-expression-natural-candidates.patch](round14-statement-expression-natural-candidates.patch)
preserves representative ordinary-C drafts. The patch applies cleanly to
this checkpoint but does **not** match the ROM and is not an accepted source
change.

### Terrain height: 1 site

`TerrainModifiers_ChangeFromScript` (`080061CC`) computes a signed height
through `heightLevel`. Assigning the call directly to `height` produces a
244-byte function instead of 248 bytes: the result is formed in the eventual
destination register rather than copied from the return-value register.
The full unit becomes 1,364 bytes instead of 1,368.

Using the real signed `TerrainVec` position, lifting the input coordinate,
and using narrow height return types do not restore the match. A block that
preserves the existing result alias does match, but its only remaining role
is to preserve that source-shape dependency. It was deliberately withheld.
Moving the alias out of a statement expression would hide the debt.

### Golem route status: 1 site

`Golem_TryRoute` expands repeatedly inside `Golem_UpdateHitReaction`
(`080C9C04`). Direct sequential route checks change 32 bytes in the unit.
The best ordinary draft combines the navigation predicate and the real
terrain-status assignment in a short-circuit condition. It changes 11 bytes
in the 12,492-byte unit; the affected function remains 8,764 bytes.

The remaining differences are eleven stack-slot references. For example,
an actor-field address saved at `sp + 0xDC` in the ROM is saved at
`sp + 0xE4` in the draft. Later reads move consistently with it. The changed
slots hold field addresses introduced by PRE and spilled by reload; the
route calls and control decisions agree. The source structure that gives
the original slot assignment is still unresolved.

Other natural trials included the existing shared early-return route
helper structure, nested tests, a dedicated route-check inline, separate
navigation and terrain results, and a conditional result expression.
None matched. Lifting all argument aliases into a plain block also gives
eleven differences and would not remove the underlying source debt.

### Dog cell centers: 2 sites

`Dog_CellCenter` expands in `Dog_UpdateContactRecovery` (`080E0750`). Direct
fixed-point center expressions change 88 bytes in the 37,088-byte unit;
the function remains 4,232 bytes. The diff is dominated by changed stack
slots for saved actor-field addresses. For example, one store moves from
`sp + 0x60` to `sp + 0x5C`, and related saved addresses shift with it.

Trials included ordinary coordinate calculations, computed-output locals,
narrow grid-coordinate types, multiplication for the fixed-point scale,
and an actual coordinate-conversion inline. None restored the exact unit.
Preserving the old input aliases in ordinary blocks does match, but that
candidate was withheld because the aliases still exist only for their
effect on compilation.

### Mummy cell centers: 2 sites

`Mummy_CellCenter_path` expands in `Mummy_SelectMovementResponse`
(`080DA97C`). Direct conversion changes 22 bytes in the 49,808-byte unit;
the function remains 6,316 bytes. The differences are stack-slot references
for saved field addresses, such as `sp + 0x5C` becoming `sp + 0x60`.

The unit already contains real coordinate conversion and store inlines
used by another cell-center helper. Reusing those in the path helper was
tested, as were ordinary coordinate locals, narrow grid-coordinate types,
and fixed-point multiplication. None matched. The alias-preserving plain
block is exact, but was withheld for the same reason as the Dog candidate.

## Review of earlier accepted lifts

The previous five-unit pass was rechecked against the same source-quality
criterion. Its SceneObject locals perform actual flag tests or compute the
passage-state boolean. ItemContainer keeps actual flag addresses and masks
while removing its former value-copy expressions. PaletteController removes
the `hint` alias completely. ScriptRecord uses an indexed modifier traversal
and removes its duplicate cursors. Those accepted lifts do not replace the
reported syntax with a newly concealed alias-only scaffold.

Runtime candidate and check records for this pass remain under
`build/round14-fields/statements/`. The committed ordinary-draft patch and
the source-evidence note preserve the material needed to continue without
relying on those transient build artifacts.
