<!-- Archived worker evidence. Original scratch-relative paths below are provenance; current guarded source is committed under src/fn/. See ../README.md. -->

# Target-motion handoff: stock9, physical sequence exact

Tags: #player #matching #compiler

Stopped at the user's request to wrap up and retain quota. Latest quota:23%left.
No tracked files changed or commits made by this worker.

Closest stock source: best.c, identical to plain_pos_simple_frame.c.
Player_UpdateTargetMotion (0814B1F8):4176/4176B,9differing instructions,
10differing bytes,shape0,99.6%fuzzy. This remains NONMATCHING. Independent stock
check: build/check/run-fgdpxgyp/comparison.json (plus full.diff). Candidate SHA256:
9c61cfb3efa701970712d342e8b9b252bc2b95b551103ffc25c15fd89d0917d0.
The inherited fuzzy82.4comment in the scratch source is stale; check proves99.6.
Optional declarations_review.c preserves the same scope9with primarycallee
signature repairs; root should review those when landing. It is not a match.

## Decisive source properties retained

- Four particle vectors use BLK unions (ContactPosition plus genuine XYZarray),
  restoring direct halfword yupdates instead ofDIextract/merge operations.
- Separate random sample from centered radius; capture phase1 Zspeed before
  lookup and compute verticalSpeed fully before its halfwordstore.
- Phase0 and plain animationmacro use perbranchowner/data locals, a fresh
  common flag-channel pointer and direct common Anim_Step arguments. This
  restores localowner4/data5 and prevents wrong PREowner sharing.
- Direct sineZread lets PRE synthesize allfour tablecaptures inr9. Explicit
  local sine alias prevented the desired PRE and coalesced tor2.
- Phase2 direct vel2finalY/Create fields let PRE synthesize r5capture after
  tablecapture. Explicit resultalias forced the wrong instruction order.
- Orbit offset is an independently owned ContactPosition aggregate, rather
  than an old union.value access. Both6BXYZand8Bplainrecord work; extent alone
  is not the cause. Preferred source uses plain8BContactPosition with no casts.
- Phase7 frame captured before owner; phase0 frame now plainu16. Phase1
  unsigned16bitfield boundary remains required.

## Exact remaining difference

One reload rotation produces all9rows in phase5:
- BFA8/BFAA: heading930literal/add usesr5,ROMr1.
- BFEE/BFF0: side931literal/add usesr1,ROMr2.
- C01C/C01E: frame1114literal/add usesr2,ROMr3.
- C038/C03A/C03C: positionaddress108materialization usesr3,ROMr5.
AGBCC_SPILL=137:1 (or137:2) gives FULL MATCH on the closestsource. Diagnostics
only; no knobs, pins, barriers or compiler overrides are in the source.

## Causal proof and next source hypothesis

The missing history is a hidden outputreload0 in the earlier phase4flagupdate
block that jump2 later deletes. Its flagsvalue1930 is currently native0, set by
load andAND. Private eviction before reload forces the missing outputreload,
fixesall9rows, but allocates a4Bslot and changesframe2Cto30. The physical flag
block disappears; only frame differs. Evictpositive1927 proves the same.

A private validated NOINHERIT hook, disabled on295candidateinsns andthenALLuids,
producesidentical9. Earlier SPILL97–136 rotations never match. Thus inheritance
andearlierexistingchoice rotations are ruled out. Proofs are saved in
round5-helper/inherit-review/ (identity, validation, spill97-137, evict1930).

Separate constold/new flagvalues keep one-set result throughcombine, but
regmove reties them. NOTIE plus evictresult alone leaves9 because reloadshares
nativeinput0 without advancing. Evictbothinput/result fixes9 with noframegrowth,
but masksetup movesbeforeload and prevents crossmerge (3ROM/5draft,shape8).
See inherit-review/notie-evictions.json and notie-full/.

Next supported source hypothesis, NOT EXECUTED: a normal do-zero READ_CONTROL
macro loads oldFlags, then a separate result/store outside the wrapper. The
loop-note boundary may prevent regmove retie while keeping the ANDresult one-set
and eligible for target-memory REG_EQUIV. This is analogous to existing shared
ParticleRandom readmacro. Keep output setter outside wrapper to avoid weighted
N_SETS2. Reuse dead orbit-offset slot if an addressable controltemporary is used.

Raw/bitfield32/onewordarray containers collapse to stock9. BLKcontainers retain
an extra dead stackstore, blocking crossmerge. Low31/24bitfield+reserved-high
views preserve semantics but keep mask/merge operations; no candidate is selected
from those. Avoid redoing the many variants recorded in REPORT.md and helper's
REPORT.md. Root owns source/includes/commits and batch validation.
