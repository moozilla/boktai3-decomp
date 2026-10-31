# Round 7: Ghoul disc states

Tags: #actors #matching

Both disc states now match in ordinary C. Their callback references and function
names were updated with the matching source.

| Address | Function | Exact bytes | ROM and candidate SHA256 |
|---|---|---:|---|
| 08085DD0 | `Ghoul_UpdateAimedDiscThrow` | 1524 | `3701da330ede36a1cf8631610fd77a9c139e3eae695b7f03cb2d5162389000fe` |
| 08089340 | `Ghoul_UpdateDiscThrow` | 1216 | `263c009464187f8343aa758c490c3094b8534e5cf512e78325f395d362a0740d` |

## Row scan ownership

The recovered scan already had the correct successful width reads and later
cell-center loads. Its remaining differences came from the initial map-slot
lifetime and the four attack-field addresses prepared before each row loop.
The saved reload diagnostic proved the complete 08085DD0 byte match, but was
never added to the C or build configuration.

Reacquiring `terrain = &gTerrainMap` at the two interrupted exits removes the
initial slot's call crossings. With `do` loops, however, the initial value dies
before the width load's scratch register and the field-address computations.
Writing the south/north scans as ordinary `for` loops causes the loop pass to
move the shared map-literal computation into each preheader. CSE then copies
the initial `terrain` value into that temporary after the four field addresses.
The copy disappears during reload, while its earlier lifetime preserves R1
through those computations. The saved exact trace records this as instruction
1907: a literal load in `loop`, a copy from native `terrain` in `cse2`, then gone
in `greg/reload`.

The east/west scans retain their `do` loops: converting those to `for` changes
their branch order. The private scan helper receives a positive step count;
both callers pass one. The identical recovered helper matches both functions.

## Validation

Stock `tools/check.py` checks passed before and after naming, with the reviewed
extents above and zero different bytes. Commands:

```sh
build/venv/bin/python tools/check.py src/fn/sub_08085DD0.c --target-size 1524 --json
build/venv/bin/python tools/check.py src/fn/sub_08089340.c --target-size 1216 --json
```

The continuation recovery snapshots retain the candidates, exact stock JSON,
and compiler traces under `build/round7/early/08085DD0` and
`build/round7/early/08089340`. Combined ROM verification remains the integration
gate. No pins, assembly barriers, diagnostic overrides, or new selectors were
introduced.
