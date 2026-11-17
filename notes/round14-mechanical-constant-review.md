# Round 14: direct constant values across the remaining source units

Tags: round14, cleanup, concessions, const-local

Date: 2026-11-16. Baseline: `b6ac18174`. Accepted source commits:
`793cf84b7` through `3af1eae41`, one source unit per commit.

## Accepted changes

Remove **263 scalar constant locals in 215 units**, replacing their reads with
the actual constant. Declarations and standalone constant assignments disappear;
the code that uses the values remains in its existing order. Unsigned words use
`0u`/`1u`, preserving the type of shifts, arithmetic and bitwise expressions.
Narrow integer types retain their ordinary integer promotions.

These edits add no replacement variable, helper, loop, identity, cast, union,
compiler setting or suppression. Other existing constructs remain visible.
In particular, this pass does not rename `zero8`, remove unrelated unions, or
claim that an accepted unit is otherwise free of allocation machinery.

The source-only linter change is **263 fewer `const-local` warnings**. Every
other kind is unchanged. Starting from the integrated worker snapshot,
`const-local` changes from 1,306 to 1,043 and the full warning total from 1,828
to 1,565; the three existing volatile errors remain.

## Search boundary and open work

The fixed hypothesis was direct replacement of a scalar that only receives
zero or one. It covered **735 candidate declarations in 557 units** outside
the other workers' owned files. A whole-unit candidate was checked first,
followed, when necessary, by function groups and individual variables. This
was not a search over spellings, declaration order or replacement scaffolding.

The remaining **472 candidates stay open**. A failed direct substitution is
only an initial measurement: it does not prove that the original source needed
the temporary. Some grouped candidates exposed an empty control structure or
did not compile and were rejected. The more detailed animation and enemy notes
examine recurring load-order, field-type and allocation decisions.

Address-taken words, pointer locals, dynamic values, ambiguous shadowing,
aggregate fields and mixed assignments were excluded from this mechanical
pass. Their omission is not an exemption or an authenticity finding. The
adjacent `round14-constant-removal-audit.tsv` lists every considered declaration,
its baseline line, and whether it was removed or remains open.

## Validation and review

Before selection, each candidate's complete linked unit was compared with the
unchanged stock unit; the stock baseline was independently compared with the
ROM. After review and whitespace cleanup, all **215 final candidates**, covering
**288,036 bytes**, passed `tools/check.py` with the original unit's full extent
supplied explicitly. Neither source nor compiler warning counts increased in
any accepted unit.

The first 108 files received a separate worker review; the root reviewed the
remaining 107. Review covered declarations versus member names, shadowing,
constant-only writes, conversions, addressable storage, and empty control
structures. Only lines emptied by deleted statements were removed; existing
comments and blank lines were preserved.

Ignored reproduction artifacts are in `build/round14/const-pass/`: baseline and
final candidate source, per-unit checks, attempted combinations, validation
results and the accepted commit map. The complete stock ROM check belongs to
the combined delivery report.
