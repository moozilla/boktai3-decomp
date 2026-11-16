# Pilot drafts (depin-slim-2), not yet matching

Each file is a whole unit draft; compile with `python3 tools/scope/scope.py FILE FUNC`.

* `SceneActorContacts_ReleaseInvalidOwners.c` (08072D34): 8 diffs, clean C. `AGBCC_FORCE` of `valid` (the `(u16)value` result) to r1 matches. The flags byte (gcse's reaching register) loses its copy to r4 because the second site's `u16 clear = node->flags & clearMask` is not PRE-replaced; the `node->flags &= clearMask` form is replaced but then `valid` is allocated before `node` and takes r2. Needs `valid` born later/shorter-priority with the `&=` form, or a PRE-matching u16 form.
* `sub_08108C94.c`: 9 diffs. `(u8)first` in the loop adds pre-combine insns so `matches` (REG_EQUIV-doubled life) outranks `base`. Left: `count` (689) vs the hoisted field6f address (681) needs ~2 pre-combine insns before the loop after `base` is set; then the cse canonical order of `v` and the 0xFFC0 AND.
* `sub_081D3620.c`: 12 diffs. Local-alloc in the loop block ranks the coordinate chain (12 refs, life 8) above the sample chain (8 refs, life 8); refs are counted after combine, so filler does not help. Needs a different statement shape.
* `SceneActorPalette_ApplyTransition.c` (08071F50): 28 diffs, only stack-slot order (gcse PRE pseudo order) is wrong.
