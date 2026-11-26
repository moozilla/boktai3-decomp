# Round 14: proved-zero reports from compiler expansion intermediates

Tags: #matching #tooling #compiler #concessions

## Result

Three reported source sites are false positives for the claim that their C
expression is identically zero. The detector records a zero-valued
intermediate introduced while lowering a live operation, then attributes
that intermediate to the enclosing source statement.

| Source at baseline `c940902f6` | Reported operations | Nonzero source case |
| --- | --- | --- |
| `src/bosses/hresvelgr/sub_0819FC34.c:131`, `Hresvelgr_UpdateTurningCharge` | `ashiftrt`, `lshiftrt` | Sample 8 gives remainder 1 and turn +16. |
| `src/bosses/vanargand/sub_081A9DDC.c:47`, `Vanargand_UpdateAlternatingPairAttack` | `ashiftrt`, `lshiftrt` | Sample 8 selects half 1. |
| `src/monsters/vorn/vorn_main.c:2117`, `Vorn_BeginPursuitRoute` | `and` | Initial flags 0 become 4, then remain 4. |

These are **three warning sites and five raw diagnostic records**. They
share a loss of operation provenance. Two distinct lowering paths generate
the intermediates: signed division correction and halfword-field update
machinery. This does not establish that every source form around these
expressions is original or free of other source debt.

No production source, shared header, detector, or canonical compiler changed
in this audit. No ignore was added and no warning reduction is credited.
The natural shared-RNG source candidates below are nonmatching and remain
unapplied. Vorn source changes are owned by the root/environment workers.

## Signed remainder: the zero is its sign correction

Both private `RandomValue` helpers return `s32`, but their final load is an
unsigned halfword. Their possible values therefore lie in `0..65535`.
The source computes `(RandomValue() >> 3) % 2`.

In `expmed.c:expand_divmod`, the signed power-of-two division path generates
a correction before shifting the quotient. For divisor 2, the auxiliary
operations are an arithmetic right shift by 31 followed by a logical right
shift by 31. The remainder is subsequently calculated as
`x - 2 * (x / 2)`. The original source shift is by 3; it is a separate
instruction and is not identically zero.

The actual no-`-g` emitted RTL establishes the following chains:

| Function | Sample load | Source shift by 3 | Synthetic shifts by 31 | Remainder |
| --- | ---: | ---: | ---: | ---: |
| Hresvelgr | 611 | 618 | 619, 620 | 626 |
| Vanargand | 96 | 107 | 108, 109 | 115 |

The numbers are instruction UIDs, not ROM addresses. A diagnostic-only
compiler copy recorded the exact expressions from accepted combinations.
For Hresvelgr, both reports are attached to UID 620; for Vanargand they are
attached to UID 109. The accepted transformation sets the correction
pseudo to zero. For example, Hresvelgr reports these two internal proofs:

```text
(ashiftrt:SI (reg:SI 279) (const_int 31))
(lshiftrt:SI (reg:SI 279) (const_int 31))
```

Here pseudo 279 is the zero-extended sample. The accepted result is
`(set (reg:SI 283) (const_int 0))`. The real shift-by-three, quotient,
multiply/subtract remainder, and turn calculation survive combine. The
corresponding Vanargand sample/correction pseudos are 65 and 72.

Independent enumeration of every halfword value gives 32,768 cases with
remainder zero and 32,768 with remainder one, while every sign correction
is zero. This also occurs with the actual initializer rather than only an
abstract input: replaying the `123456` seed, `Random_Next15`, and
`Random_InitValueTable` produces 512 of each remainder among 1,024 samples.
Table slot 1 contains 25,215 and produces remainder 1.

## Vorn: the real flag clear survives

The baseline uses `struct VornResponseFlags { s16 bits : 16; }` and first
sets bit 2 with `Vorn_SetResponseFlags(flags, 4)`, then clears bit 1 with
`flags->bits &= ~2`. For every 16-bit input, the result retains bit 2 and
is therefore nonzero. The concrete input zero gives `0 -> 4 -> 4`.

The backend's `expmed.c:store_fixed_bit_field` builds a field update from a
preservation mask and the replacement value. A full-width halfword field
has no surrounding bits to preserve. Emitted RTL includes a synthetic
AND with zero, such as UID 173, beside the real source AND at UID 166.
However, **UID 173 is already gone before combine and is not itself the
reported combine expression**.

The CSE/flow trace retains a halfword-zero residue as pseudo 66. UID 150
ORs that widened subregister into the result of setting bit 2. Combining
this with the following flag clear creates a temporary expression whose
zero arm is:

```text
(and:SI (const_int 0) (reg:SI 90))
```

The exact accepted-event trace attaches that proof to UID 166. Its
surrounding temporary pattern is:

```text
(set (reg:SI 89)
  (ior:SI
    (and:SI (const_int 0) (reg:SI 90))
    (and:SI
      (not:SI (subreg:SI (const_int 0) 0))
      (and:SI (reg:SI 76) (reg:SI 90)))))
```

After the accepted transformation, the real operation remains:

```text
(set (reg:SI 89) (and:SI (reg:SI 76) (reg:SI 90)))
```

Pseudo 76 is the value with bit 2 set; pseudo 90 is the clear mask `-3`.
The actual source flag clear was never proven zero.

The root independently tested an ordinary `s16 bits` member in place of
the bitfield. That source is exact across all 25,404 bytes of the Vorn
unit, and this worker independently reproduced the same `proved-zero`
report on its candidate. Ordinary `u16` and full owned-field/pointer
migrations did not match in the root's experiment. Thus this is not
restricted to bitfield spelling: ordinary signed-halfword storage can
leave the same backend preservation/conversion residue. The root keeps
those source candidates in `build/round14/vorn-flag-fields-pass/`.

## Boundary required for a detector correction

Suppressing remainder syntax, bitfield syntax, or an entire generated
instruction would be too broad. A genuine source zero can be combined
into the same instruction. Current positive controls still report for:

* `(sample() >> 16) % 2`, where `sample()` returns a halfword as `s32`;
* `flags->bits = gHalf >> 16`;
* `((s32)gHalf & 2) % 2`.

The two minimal signed-sampler cases reproduce their false positives. A
small isolated set-4/clear-2 bitfield does not reproduce Vorn's warning:
its preservation zero disappears earlier. Adding a context, a live zero,
or the pursuit tests alone also does not recreate the actual CSE/flow
residue. The full Vorn source remains the demonstrated regression case.

A correction needs operation-level provenance or a narrowly established
RTL rule that distinguishes these internal zero arms from the genuine
source zeros above. No such correction is adopted in this note.

## Natural shared-RNG source candidates

Before proposing a detector change, the two complete private samplers were
removed in scratch candidates and their calls replaced by the existing
`ParticleRandom_ReadDirectSample(gRandomTable)` or
`ParticleRandom_NextSample(gRandomTable, &gRandomIndex, 0x3FF)` operation.
Both are ordinary unsigned APIs. The old sample macro was not used, and
no cast was added to steer signedness.

| Unit | ROM bytes | Direct sampler | Next-sample operation |
| --- | ---: | --- | --- |
| `0819FC34` | 616 | 616 bytes; 324 differing bytes | 616 bytes; 323 differing bytes |
| `081A9DDC` | 428 | 420 bytes; 359 differing bytes | 420 bytes; 359 differing bytes |

The direct candidates have zero compiler-derived concession reports.
Scope shows instruction-shape changes, including unsigned remainder lowering;
these are not solely operand allocation differences. All four candidates
are rejected, and their disappearance of the diagnostic is not counted
as a source or diagnostic fix.

## Evidence and reproduction

`notes/round14-proved-zero-expansion-evidence.json` records the source
hashes, selected emitted/flow/combine RTL, exact accepted zero-expression
traces, exhaustive counterexamples, minimal negative/positive probes,
and the ordinary signed-member corroboration. The sources and headers
were independently materialized from canonical commit `c940902f6`;
their preprocessing, emitted assembly, and accepted traces matched the
worker observations exactly.

`notes/round14-proved-zero-trace.patch` applies only to `combine.c` in an
isolated copy of the existing instrumented compiler. It snapshots the
zero expression and enclosing pattern when the existing reporter records
an event, then prints them with the accepted pattern under
`AGBCC_ZERO_TRACE`. It changes no compiler decision. With `AGBCC_DEAD`
enabled, all three units emitted byte-identical assembly to the canonical
instrumented compiler. The canonical compiler's binary hash remained
unchanged. This patch is an unapplied tracing aid, not a detector fix.

`notes/round14-rng-choice-trials.jsonl` contains the two baseline and four
candidate records, with full source patches against `c940902f6`. Every
patch was replayed and its candidate hash checked.
`notes/round14-rng-choice-open-candidates.patch` contains the two direct
sampler candidates and passes `git apply --check` against the unchanged
source. The transient compilers and full dumps remain under
`build/round14-zero-origin/`; RNG trials remain under
`build/round14-rng-choice/`.
