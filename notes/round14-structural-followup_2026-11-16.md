# Round 14: structural follow-up

Tags: #round14 #matching #shared-operation #source-evidence #checkpoint4

Date: 2026-11-16. This note separates two adopted control-operation recoveries
from the remaining particle, behavior-packet, and private-view investigations.

## Linked idle control installation

Lun and Ghost now install the whole control packet through the existing
SceneActorChase_SetControl operation. It sets the control-entry flag, clears
completion state, installs the control kind and callback, and resets pending
control state. The canonical player-array ownership resolved the header conflict
that previously blocked Lun's shared-helper candidate.

| Unit and callback | Independently exact unit | Adopted commit |
| --- | ---: | --- |
| `lun.c`: SceneActor_BeginLinkedIdleReaction_0809C834 | 16,116 bytes | `53eb189a4` |
| `ghost.c`: SceneActor_BeginLinkedIdleReaction | 22,680 bytes | `ab29d1580` |

**These changes receive no warning credit.** Each unit retains 14 constant-local
warnings before and after the change, with zero errors. The adjacent behavior
packet still uses its existing locals. Replacing only that packet with
SceneActorChase_SetBehavior changes ten bytes in either unit. Replacing both
behavior and control installation in Lun changes eight bytes. Those alternatives
remain unapplied, and the residual behavior packets stay open.

## FadingParticleGroup_Update

Eight ordinary structural alternatives were tested against the 540-byte unit at
`08016118`: motion/lifetime simplification, indexed or advancing particle
traversal, direct particle fields, and existing shared animation operations.
Every alternative fails stock matching.

| Candidate hypothesis | Unit bytes | Differing bytes |
| --- | ---: | ---: |
| Ordinary motion and lifetime | 536 | 233 |
| Indexed particle traversal | 520 | 502 |
| Advancing particle traversal | 520 | 500 |
| Direct particle fields | 568 | 526 |
| Shared next-frame operation | 536 | 233 |
| Indexed traversal with shared next-frame operation | 520 | 502 |
| Shared complete sprite step | 560 | 517 |
| Indexed traversal with complete sprite step | 572 | 541 |

The existing source remains unchanged. These whole-unit byte counts include
offset shifts when a candidate changes length; they are not counts of distinct
semantic or allocation defects.

## Vorn_BeginPursuitRoute

Three field-representation candidates were tested against the 25,404-byte Vorn
unit. Replacing the private full-width signed bitfield with an ordinary `s16`
member is exact, but retains both the private view and the reported compiler
intermediate. **That cosmetic exact candidate is withheld.** An ordinary unsigned
member produces 25,408 bytes with 8,567 differing bytes; migration to the actual
owned flag word produces 25,400 bytes with 11,660 differing bytes. Neither is
adopted.

The [proved-zero expansion audit](round14-proved-zero-expansion-audit.md) traces
the warning to a synthetic zero arm introduced during halfword-field lowering.
The live source operation sets bit 2 and clears bit 1; it is not identically zero.
The audit also confirms that changing the bitfield to an ordinary signed member
retains the report. This diagnostic finding does not establish that the private
view is original source, and it earns no source or warning reduction here.

## Evidence and verification

[The source candidate catalogue](evidence/round14-structural-followup_2026-11-16.jsonl)
contains **16 complete source patches: three exact candidates and thirteen
mismatches**. Two exact candidates are adopted; the third is the withheld Vorn
form. The records comprise eight Fading alternatives, three Vorn forms, the
combined Lun setter attempt, and four separate Lun/Ghost control or behavior
attempts.

Their named source baselines are `c940902f6` for Fading, `99d30862c` for Vorn, and
`720eb4fad` for Lun/Ghost. All sixteen baseline hashes were checked against their
actual Git blobs; every patch was replayed and its resulting source hash verified.
The four appended Lun/Ghost records also agree with their saved candidate files
and binary-comparison results. Both adopted candidate hashes match the complete
source blobs in their integration commits.

Each patch reconstructs an independent candidate against its named baseline;
the catalogue is not a patch series. It contains source and verification metadata,
with no generated assembly or ROM data. The coordinator owns combined counts and
the complete ROM build.
