# Round 14: separate unrelated values from two local unions

Tags: #round14 #concessions #local-union

Date: 2026-11-16.

Two allocation-only unions can be replaced by independent typed locals with
unchanged stock output:

| Function | Independent values | Complete unit | Commit |
|---|---|---:|---|
| BossSlot2_ApplyPendingDamage, 0817F49C | Pending damage flags and the health-field pointer | 664 bytes | `31c9fa43e` |
| SceneActor14_UpdateAnimState | Sprite-pool manager and selected animation resource | 2,216 bytes | `09cd4fbf8` |

These are not alternate representations of a common value. In the damage
handler, the flags are consumed before the health pointer is assigned, and all
health-pointer reads occur after that assignment in the damage branch. In the
animation handler, the manager is initialized at entry and each animation
resource is assigned from it before use. There is no cross-member read or
shared-storage requirement. The original member names and types become ordinary
locals; no replacement aggregate or type pun is introduced.

Both complete units passed independent stock checks with their original extents,
and compiler/source lint confirms only `local-union` falls, by two sites. The
sprite-effect unit is now warning-free; the damage handler's two constant locals
remain visible.

## Initial review of the surrounding family

A conservative direct-split pass examined 16 flat unions in 14 units. It
excluded aggregate views, arrays, unassigned views, ambiguous names, whole-union
uses, and files owned by other active workers. Besides the two accepted unions,
the following direct splits change stock output and remain open:

- `background_layers.c`: dividend/reciprocal work value.
- `0804373C`: random mask and cursor.
- `08059DF8`: turn step and random-table pointer.
- `0805A7E4`: near/table, far/random-phase, and lift/phase pairs.
- `0805AE58`: graphics resource and random-table pointer.
- `0805CFE8`: first argument and resource pointer.
- `08080494`: sample and category pointers.
- `08128F98`: cursor mask and sampled value.
- `0812A928`: sine pointer and height mask.
- `081D0864`: flags and counter.
- `081D905C`: collider manager and position.
- `081E2910`: destination pointer and visibility mask.

These are measurements of the direct split, not exhaustive source-recovery
results. Existing deeper field/random-table investigations are recorded in
`round14-fields-warning-review.md`. Genuine packed/vector views were not
classified as concessions merely because they use a union, nor excused without
their own source review. No ignore was added.

The ignored baseline/candidate sources and validation record are in
`build/round14/union-pass/`.
