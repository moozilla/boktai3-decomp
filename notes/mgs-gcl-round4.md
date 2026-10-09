# MGS/GCL matching round 4

Sol 6.1 worker on `codex/mgs-gcl-round4` from `11bee1d`, isolated private outputs.
Started 2026-10-09 03:28:02 UTC; final validation completed by 03:44:14 UTC
(about 16 elapsed minutes). Production stopped before the 20-active-minute
bound; all matching/source-comparison findings and negatives are in
`docs/MGS_GCL_MATCHING.md`.

| New target | Native bytes |
|---|---:|
| `08219AAC` | 120 |
| `08219C40` | 116 |
| `08219CB4` | 132 |
| `08219D38` | 124 |
| `08219DD8` | 108 |
| `0821A184` | 68 |
| `0821A284` | 40 |
| `0821A520` | 332 |
| `0821AA1C` | 52 |
| `0821AA50` | 56 |
| `0821B20C` | 164 |
| `082250FC` | 552 |
| `08225448` | 68 |
| `082258DC` | 48 |

Total **14 new matches / 1,980 native bytes**. Full `tools/build.py` printed
`build/boktai3.gba: OK` with these units; no make/shared-gen writes, push,
PR, or worker screenshots. Native byte counts include pools/alignment.

Bounded searches: allocator 90s score20→0, update90s15→0,
front-heap allocator60s110→0, entry insertion90s255→0;
resource lookup75s285→95 then60s20 without improvement before independent
matching rewrites; timer setup60s10 without improvement; prior vector45s60
without improvement before independent indexed-loop reconstruction.
Timer setup remains WIP at one commutative addition operand-order difference;
trap WIP was preserved. MGS memory/cache comparisons established important
layout/algorithm differences, and GCL_StrToSV gave a useful three-value family
comparison without importing external C. No semantic names installed.
