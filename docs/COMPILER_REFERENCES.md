# Community references and checked agbcc patterns

Reviewed 2026-10-08: [Decompedia GCC](https://decomp.wiki/compilers/GCC) and
[GBA resources](https://decomp.wiki/platforms/game-boy-advance). The GCC page
mixes compiler versions and architectures. Use its patterns as hypotheses;
MIPS delay slots, PS2 floating-point pools and architecture-specific loads do
not establish what this project's Thumb compiler will emit.

The following small probes were compiled locally with the pinned agbcc and
the project's `-O2 -mthumb-interwork -fhex-asm` flags. They are useful shapes to
recognize, not replacements for checking the surrounding function:

| C expression | Observed Thumb shape |
|---|---|
| signed `x / 2` | Extract sign as unsigned bit, add it, then arithmetic shift by one. This rounds toward zero. |
| signed `x % 16` | Add 15 on the negative path before arithmetic shift; subtract the quotient times 16 from the original value. |
| `(unsigned short)x >> 3` | Left shift by 16, then logical right shift by 19. The final C shift is 3, not 19. |
| `(unsigned)(x - 14) < 9` | Subtract 14, compare with 8, branch unsigned-higher. |

Unsigned range idioms require the intended input domain to be understood;
signed subtraction can overflow at extreme values in C. Do not assume a
syntactically similar rewrite is valid for every possible input.

Prefer actual struct fields for arrays inside objects. Explicit source-value
temporaries, loop pointer induction, switch cases and branch layout can change
register allocation. Successful project examples and failures are recorded in
`docs/WORKER.md` and `notes/round1-*.md`.

## Permuter findings

The GBA page links [decomp-permuter-agbcc](https://github.com/WhenGryphonsFly/decomp-permuter-agbcc).
The reviewed fork commit was `1f7ef872b12f54db7678ff00e0346abc015410ae`.
It needed pycparser 2.22 in the local compatibility trial; installed pycparser
3.1 lacks its imported `plyparser` module. The repository continues to pin
upstream `8556c81d80d1c1af98a858c8f4dc951357f29139`, which bundles its parser.
Both versions exposed the same project-side scoring issues; switching forks
alone did not resolve them.

The wrapper now:

- Links target and candidate against one baseline symbol snapshot at the same
  ROM address. Previously a numeric RAM address versus its relocation symbol
  incurred a false penalty even for an exact match.
- Excludes trailing `.incbin` data using the same boundary policy as the ROM
  build. That data remains in assembly, so C should not be scored for omitting it.
- Preserves the preprocessed target body and static inline helpers. The old
  regex stripping script could misidentify a compact `if` body and erase it.
  Inputs with multiple ROM functions are rejected; isolate the target first.
- Honors per-file compiler flags, scores stack differences, isolates each run's
  outputs, and propagates child-process failures instead of calling them a
  search with no improvement.

Usage (activate `build/venv` first):

```sh
python tools/permute.py wip/sub_XXXXXXXX.c --debug
python tools/permute.py wip/sub_XXXXXXXX.c --run 60 -j 2
```

Each invocation prints a fresh `build/permute/FUNC/run-*` directory. `--debug`
scores only the base and shows the assembly comparison; normal search saves
candidates in that run's `output-*` directories. Run `tools/check.py` on any
promising output, then verify the full ROM before committing. A permuter score
of zero is still only a search signal: normalization can ignore branch targets
and other distinctions that the byte comparison catches.

Known matched controls `08033568`, `08139298`, and `081FB788` now score zero.
The latter previously scored 2300 due to retained trailing data. A preserved
`081A55D4` candidate now scores 20 with its loop intact; a 60-second search did
not improve it. Its former score of 3105 came from the stripped-away loop,
not from the quality of the worker's C. Local transcripts and probe sources
remain under ignored `build/research/` and `build/community-probes/`.
