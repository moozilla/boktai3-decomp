# Round 7: particle sample addresses

Tags: #actors #matching #rng

Two functions now match in ordinary C:

| Address | Function | Bytes | ROM and candidate SHA256 |
|---|---|---:|---|
| 080C725C | `Golem_UpdateSlideImpact` | 860 | `2da4837c61d1c8e82c72496cba071b746c403c3bbffae91763212b38e91eedd9` |
| 080DF908 | `SceneActorAnimation_SpawnTrailAtPosition` | 232 | `342274e00cffcefa06cb20db3904a79e3107d660f2cd0c4f5ab6dae202199683` |

Both routines retain the otherwise unused origin `0403B714`. An address-to-offset
read resolves it naturally: form the next sample address from the origin and
random index, subtract the origin, then read that offset in the physical table.
The shared helper accepts independent address, origin and table inputs. Inlining
the advancing caller cancels the origin during combine, after flow has retained
its observed load and copies. The original purpose of that origin remains unknown.

Dog captures the sample origin after its first polar multiplication. Golem's
first sample uses the literal table address; its later samples use the named
table through the relative reader. These boundaries preserve the observed
allocation without pins or assembly barriers. Golem also uses the canonical
actor, body and chase group with a typed slide context.

The identical address readers are shared in `monster_particle_random.h`.
`HEAP_HEAD_ADDR` and `RESOURCE_REGISTRY_ENTRIES_ADDR` name the required literal
terms and are checked against the link map by `tools/check_ram_literals.py`.

Stock `tools/check.py --target-size` checks passed for both full extents before
and after helper extraction, with zero different bytes. The saved worker
experiments include the prior combine-timing diagnosis and exact stock proofs.
