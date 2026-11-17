# Round 14: object, trigger and palette wrapper cleanup

Tags: #round14 #concessions #matching

Date: 2026-11-16. Trial baseline: `348240c2f` on `codex/round14-fields`.

The bounded five-unit pass removes **23 reported warnings** with ordinary C:
16 statement expressions, 3 one-shot loops, 2 address/dereference expressions
and 2 constant locals. No new helper, macro, suppression or compiler setting was
introduced. All five final units independently match their complete stock ROM
ranges, totaling **26,348 bytes**.

| Source | Removed warnings | Exact bytes | Commit |
| --- | ---: | ---: | --- |
| `script_record.c` | 5 | 2,752 | `f5837dc4c` |
| `group_objects.c` | 1 | 6,184 | `e125ab3e6` |
| `scene_object.c` | 8 | 7,236 | `0b2d04e3a` |
| `item_container.c` | 8 | 7,024 | `e364128ca` |
| `palette_controller.c` | 1 | 3,152 | `49b8fe188` |

The final five-unit scan reports 0 errors and 19 warnings: 10 once-loop,
3 const-local and 6 local-union. It reports no compiler warnings. This is a
reported-site reduction, not a claim that these units contain no other
allocation-only source; the pre-existing dust address macro, for example,
remains unresolved.

## Successful source structures

**Scene-object and trigger conditions.** Replacing a statement expression by a
single bit test changes when agbcc loads its mask. The matching ordinary form
places the existing mask in a normal C block, with nested conditions preserving
the original short-circuit evaluation. It adds no operation, fake use or
weighting construct. All eight reported scene-object expressions are removed.
The two trigger player-action checks use the same direct conditional structure.

**Trigger values and projections.** Direct member expressions replace the
single-value expression blocks and the address/dereference of z. The region
comparison now uses the existing `GameState.region` field rather than its raw
0x5a0 address. Together with the two conditions, all seven reported trigger
statement expressions and its addr-deref site disappear. Replacing the remaining
manual projected-array address by `p->projected[i]` was also tried, but changes
the unit to 7,032 bytes and does not match; that separate simplification was not
accepted.

**Scripted modifier traversal.** Merely removing `*(&actor)` makes PRE hoist the
modifier-count address, introduces an r8 save and adds eight bytes. Replacing the
duplicate advancing entry/cursor pointers with the actual indexed modifier array
loop matches all 108 bytes of Actor_RecomputeScriptedValue and removes the
artificial read. A single advancing pointer does not match. The full 2,752-byte
unit is exact with the indexed loop.

**Independent guard and value simplifications.** Each of the three global/owner
modifier gates can lose its first one-shot wrapper exactly. Removing both wrappers
in any of them swaps the modifier and input registers (7 / 7 differing
instructions). The accepted source leaves the second wrapper visible.
Modifier_InitDrainState and BoardPieces_StartConvergence use direct zero stores.
The palette transition pointer is a direct address of the real member.

## Remaining decisions and bounded attempts

* The three modifier gates still need a natural explanation for their final
  input/modifier allocation order. Direct parameters, plain early-return gates,
  literal masks and the existing shared amount helpers were tried. None removes
  both wrappers while matching. No extra wrapper was introduced.
* BoardPiece_Wander's four nested one-shot loops still influence table/cursor/mask
  allocation. Direct table initialization produces 13 / 13 register differences
  in the same 376-byte body; the existing direct global random sampler produces
  16 / 18 differences. The four loops were retained, not replaced by another
  weighting device.
* Palette_BlendRange's loop removal produces 7 / 7 register differences. Actual
  halfword color/mask types and direct color masks were tried without a match.
* Removing Palette_InitializeFromScript's outer loop swaps the wave pointer and
  index registers (25 / 25 differences). Ordinary indexed source/destination
  palette arrays produce the same result as the cursor version.
* Removing the labeled one-shot loop in Palette_SetBrightnessFromScript changes
  22 instructions' registers. Consistent transition-field addressing does not
  restore the allocation. Only the independent pointer expression was removed.
* Scene_SpawnDirectionalDust's private sample-address statement macro was also
  tried as direct array addresses. That nonmatching draft remains a holdout;
  it was not hidden behind a new macro or header name.
* Direct removal of the three remaining constant locals changes 10 bytes in
  StatModifier_InitializeCallbacks and 4 bytes in each of the two trigger script
  constructors. Those declarations remain visible.

Runtime drafts and reports are under `build/round14-fields/wrappers`. The accepted
source commits and this tracked note preserve the outcome; no unmatched draft
was substituted into production source.
