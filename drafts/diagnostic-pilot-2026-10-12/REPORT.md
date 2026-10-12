# Diagnostic pilot, 2026-10-12

Branch codex/pilot-sol-diagnostic-20261012, base 3c975f2. Start 07:58:57 UTC; matching stopped 08:04:15 UTC (5m18s). No exact matches, zero new exact bytes, no commits, production source unchanged.

Audited native replacement extents including internal and final pools:
- 08160EA4..081612C8: 1060 bytes
- 081616D0..08161AF4: 1060 bytes
- 08161AF4..08161F18: 1060 bytes
- 08161F18..0816233C: 1060 bytes
Total family extent 4240 bytes. Each has its own prologue, return path, and terminal pool; no separate retained body in these extents. Only representative seed was compiled. Siblings were not expanded because seed did not become exact.

Recovered best seed stays 1060 bytes, five differing bytes at four instruction locations: 08160ECE/08160ED0 (native table base r3, candidate r1), 08160EE2/08160EE4 (native angle offset scratch r4, candidate r3). Best retained seed SHA-256 cf35be38b1bb1ee6a4f7ab1f8ee13a38f83afb950c4e49361d83fd5b1b354822.

Specific allocation-stage evidence: seed.i.lreg insn 55 loads table symbol into pseudo 45, which is allocated/coalesced into r1 in seed.i.greg; it is not an inserted reload. Angle access starts as insn 80 = p + constant 1420, then global reload inserts insn 1145 loading 1420 into r3. The two defects therefore have distinct compiler-stage origins. All tested controls keep table r1 / offset r3 in their global reload dumps. This narrows the failed hypothesis; it does not prove an original source shape.

Three bounded hypotheses, seven source probes:
1. A narrow RNG sample might alter Mod-argument coalescing. u16/u32 locals with array indexing both emit 1060 bytes but 8 differing bytes, moving table load before index shift. The raw-index u16 control returns to the original 5 differing bytes. Width and named-sample lifetime did not change allocation; array evaluation order explains the extra mismatch.
2. A named angle pointer slot or named integer offset might keep an allocatable pseudo instead of reload-only scratch. Both emit the baseline 1060 bytes/5 differing bytes; optimized regmove still has p + constant 1420 and global reload still chooses r3. Both forms fail to preserve a useful offset lifetime.
3. An inline RNG return boundary might alter argument-return coalescing; s32 and u16 returns both emit baseline 1060 bytes/5 differing bytes and keep the same final table/offset hard registers. Neither introduces a productive surviving lifetime.

16 exact-check executions: eight through candidate.py (seven probes plus recovered seed) and eight direct compact check.py runs (initial seed plus seven probes). Six diagnostic -da compilations: seed and raw-sample, pointer-slot, integer-offset, inline-s32, inline-u16 controls. No flag/compiler/verifier edits; no permuter or unrelated fallback. Compact hunks helped: only two short hunks were needed for the representative, making the stage-specific analysis easier than a full 1060-byte listing. Current typed retained source was recovered as reference but not claimed as newly tested.

Full build/venv/bin/python tools/build.py completed with build/boktai3.gba: OK; output build/baseline-build.log. No source was promoted. No second full build needed because production inputs never changed. All C and this report are local wip/diagnostic/; manifest.csv has source hashes. Detailed check/candidate and diagnostic dumps remain ignored under build/, and must not be committed. Stop early because none of three supported hypotheses improved the seed.
