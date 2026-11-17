# Round 14: address-expression cleanup checkpoint

This checkpoint covers the address-expression worker's source commits through
`640ef64cb`, based on `9b40f4fdd`. It removes **21 `addr-deref` warning sites and
two allocator `const-local` sites in 18 translation units**. The integration
branch incorporated these changes by `a21e52f43`, where the coordinator reports
a passing complete stock build. Work after this checkpoint is counted separately.

## What the initial measurement established

The initial inventory contained 74 `addr-deref` sites. Other workers owned 15
of them; this worker tested the remaining **59 sites in 52 translation units**.
The first experiment replaced `*(&local)` with `local`, preserving the surrounding
expression. Units containing several sites were also tested site by site after
the combined replacement failed. Only two sites matched this direct deletion:
`Vanargand_WaitForActionRelease` and `OrangeSpine_FireProjectile`.

That left 57 sites requiring investigation, rather than proving 57 unavoidable
concessions. The next pass examined the operations represented by the expressions:
flag merges, vector fields, indexed pools, signed identifiers, callback selection,
and random-table samples. Ordinary source changes in those families removed
another 19 sites. **38 sites in this worker's original assignment remain at this
checkpoint.** Counts here exclude other workers' changes and compiler diagnostic
corrections.

## Accepted source changes

Every row passed an unmodified stock-compiler comparison over the entire
translation unit. The extent is the original object's complete `.text` size,
including adjacent functions and literal pools.

| Source unit | Ordinary operation recovered | Address sites removed | Exact extent | Worker commit |
|---|---|---:|---:|---|
| `fn/sub_081ACAC4.c` | Directly clear the pending-action field | 1 | 156 bytes | `7b81cab1a` |
| `fn/sub_0820F078.c` | Direct position copy | 1 | 296 bytes | `e18d99cfe` |
| `fn/sub_0807F710.c` | Merge the existing flags into the pending-object flag value, then store it | 1 | 268 bytes | `45e1e7ec3` |
| `slime.c` | Merge the existing flags into the action-script flag value, then store it | 1 | 15,436 bytes | `342b4be70` |
| `scene_actor_lifecycle.c` | Use the byte-sized shadow kind after storing the incoming argument | 1 | 1,092 bytes | `e62339b60` |
| `fn/sub_0805CBD4.c` | Apply random offsets directly to the two position components | 1 | 112 bytes | `393eddaee` |
| `fn/sub_08060660.c` | Initialize the actual second position's vector fields | 1 | 168 bytes | `afc0bb6e3` |
| `fn/sub_08060708.c` | Initialize the actual second position's vector fields | 1 | 168 bytes | `8cf1ce015` |
| `fn/sub_08060D0C.c` | Initialize the actual second position's vector fields | 1 | 168 bytes | `14d5fb88c` |
| `fn/sub_08060EA4.c` | Initialize the actual second position's vector fields | 1 | 168 bytes | `84cc01fd7` |
| `fn/sub_08066B28.c` | Destroy attachments through their indexed part array | 1 | 144 bytes | `c861990d5` |
| `fn/sub_08199C20.c` | Represent the slot position with the existing shared `ContactPosition` type | 1 | 84 bytes | `5c986bf8c` |
| `fn/sub_080506CC.c` | Store the selected callback in the corresponding flag branch | 1 | 272 bytes | `10a246b7e` |
| `fn/sub_0804194C.c` | Read the actor's signed halfword identifier through a nullable accessor | 1 | 212 bytes | `755ccc551` |
| `staged_effect.c` | Keep halfword random-table samples as `u16` in three shake paths | 3 | 1,992 bytes | `8fc793d79` |
| `roaming_object.c` | Keep halfword random-table samples as `u16` in two wander paths | 2 | 3,352 bytes | `8a24f7f7e` |
| `fn/sub_0812A24C.c` | Allocate and clear the selected entry through the typed slot array | 1 | 96 bytes | `1ddaac88c` |
| `fn/sub_0812B084.c` | Allocate and clear the selected entry through the typed slot array | 1 | 100 bytes | `640ef64cb` |

The final two rows also remove one constant-`one` local each. The slot selection,
occupancy test, clear size, and stored slot index now follow the existing pool
types. No shared header changes were needed. The integration commit corresponding
to worker `7b81cab1a` is `e09d202e6`.

The source rationale matters independently of matching. For example, the flag
changes express a real read/merge/store; they do not add a zero expression to
change register allocation. The random samples are actual halfword table values;
the downstream shake and heading fields preserve the intended narrowing. The
callback-slot change uses the game's existing position structure instead of a
copied pair of words and a transient component pointer.

## Validation and limits

The source of acceptance is `tools/check.py` with the pinned stock agbcc compiler
and an explicit original translation-unit extent. Scope comparisons were used to
investigate candidates, but a normalized instruction score was never sufficient
for acceptance. One unsuccessful direct deletion in `collision_controller.c`
has zero normalized instruction differences yet changes a branch destination
byte; the complete stock comparison correctly rejects it.

Focused concession scans before and after each accepted source change confirmed
that no diagnostic kind increased. The 21 address-site and two constant-local
reductions are source changes, independent of the coordinator's separate fix for
provisional combine diagnostics. No warning suppression, compiler option change,
fake arm, weighting loop, or replacement address barrier was introduced.

Open sites include script-reading loops, typed pool scans, RNG cursors, position
and collision updates, callback arguments, palette fades, and RFU control flow.
Several direct deletions only change register assignment or operand order; others
change loop optimization or branch layout. A failed direct deletion alone does
not distinguish those mechanisms and does not establish that the current form is
the best recoverable source. Investigation of ordinary operation structure is
continuing.

## Reproducible evidence

The ignored evidence directory in the address-expression worktree is
`build/round14/addr-deref/`:

- `inventory.json` records the 74-site inventory and ownership exclusions.
- `results.json` records all 52 units in the direct-deletion experiment, with
  original extents, per-candidate stock results, and check-artifact paths.
- `src/` retains the original sources, direct candidates, and their check logs.
- `families/` retains complete structural candidates and accepted stock/linter
  logs. `family-matches.json` maps accepted candidates to their source commits.
- `verify_land.py` performs the full-extent stock comparison and focused
  before/after diagnostic comparison before committing a source unit.

For this checkpoint, restrict `family-matches.json` to commits through
`640ef64cb`; later entries belong to the next batch. The initial two direct
matches are recorded in `results.json` and their individual source commits.
The accepted forms and rationale are durable in the commits listed above.
