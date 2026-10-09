# Solo family tranche, October 9

The user authorized new work after the KNIDL review, alone, until 15% of the
plan remained. No agents were started or resumed. The meter moved from 18%
to 17% during matching and to 16% before closeout. Rounded account readings
are not a per-model cost measurement.

46 exact functions add **7,424 bytes**, reaching **330,514 / 2,415,354
(13.684%)** and **4,835 / 11,083 functions**. The 20% target remains short
by 152,557 bytes. [Accepted source ledger](solo-family_2026-10-09.csv)
records each function, reviewed body size, source path and hash.

## What worked

- Reuse a coherent object layout across initializer, rotation, interpolation
  and transition routines. Typing the full motion object made 0818D754
  (396 bytes) and 080F57E8 (380 bytes) exact on their first typed checks.
  0818D960 and 080F59E0 then matched after reversing the operand order of
  three ordinary sums. The same fields seeded eight more motion siblings.
- For 60-byte element arrays, keep an independently advanced element pointer
  alongside typed indexed owner fields. Advance the pointer before the index
  when that is the observed instruction order. This solved the 120/124-byte
  array initializers; making every access indexed or every access raw did not.
- Assign the random-table pointer after setup calls and use a signed widened
  temporary for the right shift. This solved related 172/128-byte initializers.
- Reuse constructor arguments, ordinary vector bitfields and small inline
  setup helpers. Inline helpers changed temporary lifetimes without binding
  registers or writing instructions. Clone reuse added 081DB324 and 082381C8.
- Favor complete small families over allocation residues. Typed interpolation
  and color routines added 08179650, 08165F78, 08055828, 080F29C4, 0811C41C
  and 080F1D44 with zero to two corrections each.

## Boundary review

The compiled .text size was checked against each accepted progress span.
081DB1D8 emits **116 bytes**, while its old 332-byte span included two
separate allocation wrappers. The retained bytes at 081DB24C and 081DB2B8
have independent prologues, allocation/setup calls, return paths and literal
pools; each occupies 108 bytes, ending at 081DB2B8 and 081DB324 respectively.
Both are added to the reviewed build/progress boundary metadata and remain
assembly. This removes 216 bytes of potential false credit and adds two
inventory functions. No generated assembly was modified.

## Retained work and failed hypotheses

Candidate C, including alternate experiments, is preserved outside src/ on
[codex/wip-drafts-2026-10-09](https://github.com/moozilla/boktai3-decomp/tree/codex/wip-drafts-2026-10-09/drafts/retained/wt/push20-integrate/wip/solo-family).
The archive manifest supplies original paths and SHA-256 hashes. These sources
are checkpoints, not accepted code or fuzzy progress.

- 08160EA4 still emits 1,060 bytes with four differing instruction locations.
  A typed object and compiler RTL dumps did not change the table/offset scratch
  register allocation. The bounded KNIDL -fprologue-bugfix probe also did not
  improve this target or the controls; the production compiler recipe is unchanged.
- typed_flags_08235C30.c emits 312 bytes with ten differing instruction
  locations in the flags-pointer selection/clear. Its projection prefix and
  remaining tail match. Typing the owner or naming the loaded flag value did
  not solve the remaining register choices.
- interpolate_0.c for 0823350C retains the 228-byte near-match with an R5/R6
  swap. Later helper variants did not improve it.
- 08234810, 08234868, 08234DC8, 082358C0, 082359CC and 08235B34 retain
  ordinary algorithm/layout drafts; counter truncation, pointer lifetimes,
  or allocation still differ. Do not treat their matching portions as stable
  independent C credit.
- New 0818DB10/080F5B68 vector updates, 0818CF10/0818CFEC and
  080F4FA8/080F508C return-motion patterns remain assembly. Their retained
  candidates capture the shared layout; constant reuse and flag-store order
  remain unresolved. The latter two have no new C draft in this tranche.
- Palette candidates 08111578 and 08165D10, and large setup candidates
  081BF67C/081DEE38, remain unmatched. Union/inline color extraction and
  typed resource fields did not yield an exact source. The typed 081E013C
  probe did not resolve the retained setup candidate. The 080AB294 u16 RNG
  and table probes did not fix its table/counter lifetime differences.
- The loose clone pass found no more candidates after the new motion seeds.
  A saved-register-normalized family search tried one candidate and matched
  none. Temporary search scripts and compiler dumps remain ignored.

## Validation

The combined original ROM is bit-identical. One shifted ROM at +64 KiB is
reused for all three scenarios: intro 16, save boot 7, menus/field 12
screenshots, all identical. One shifted menus harness run stopped with a
SIGSEGV after seven screenshots; rerunning that scenario completed normally.
The shifted ROM SHA-1 is identical to the previously validated main shifted
ROM (c208e82ae4996ba6e2066a724421190f5cf26a02). The original build is restored and prints
build/boktai3.gba: OK before final commits. No broad tooling test suite was
repeated. Exact functions retain individual one-translation-unit commits;
main receives one reviewed integration batch.
