# Pilot drafts (depin-slim-2), not yet matching

Each file is a whole unit draft; compile with `python3 tools/scope/scope.py FILE FUNC`.

* `SceneActorContacts_ReleaseInvalidOwners.c` (08072D34): 8 diffs, clean C. `AGBCC_FORCE` of `valid` (the `(u16)value` result) to r1 matches. The flags byte (gcse's reaching register) loses its copy to r4 because the second site's `u16 clear = node->flags & clearMask` is not PRE-replaced; the `node->flags &= clearMask` form is replaced but then `valid` is allocated before `node` and takes r2. Needs `valid` born later/shorter-priority with the `&=` form, or a PRE-matching u16 form.
* `sub_08108C94.c`: 4 diffs. Pre-combine filler fixed global-alloc order: `(u8)first` in the loop (matches over base, both lives REG_EQUIV-doubled), the `none` set moved after `base`, and an inline u16 getter for `obj->flags` before the loop (count under the hoisted field6f address). Left: cse's canonical choice between the peer load and its copy (ROM: load r3 feeds the 0xFFC0 AND, the copy r2 feeds the compares) and the AND tie (`AGBCC_NOTIE=193` gives 2 diffs).
* `sub_081D3620.c`: 12 diffs. Local-alloc in the loop block ranks the coordinate chain (12 refs, life 8) above the sample chain (8 refs, life 8); refs are counted after combine, so filler does not help. Needs a different statement shape.
* `SceneActorPalette_ApplyTransition.c` (08071F50): 28 diffs, only stack-slot order (gcse PRE pseudo order) is wrong.
