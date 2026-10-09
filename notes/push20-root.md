# Push toward 20%: parent matches

Base `5ff44e3`; isolated branch `codex/push20-integrate`. User authorized Sol 6.1 workers at higher reasoning, bounded Astra-low escalation only for blockers, and a new 20% remaining usage floor. Started with 32% remaining and 296,658 matched code bytes.

The first accepted parent batch adds 11 functions / 3,812 emitted bytes:

| Functions | Bytes | Evidence |
|---|---:|---|
| 0824AC40, 0824AC74, 0824ACE4 | 188 | Pinned pret/agbcc libgcc2 source specialized and checked with old_agbcc -O2 |
| 0824930C | 68 | Same source and compiler: unsigned double-to-word conversion |
| 08229618 | 2,064 | Natural double series reconstructed from resolved math calls; first draft MATCH |
| 08229E28, 08229560, 08229FEC | 652 | Adjacent arithmetic/trig functions; first drafts MATCH |
| 08229FFC | 564 | Two bounded refinement loops; first draft MATCH |
| 08229468 | 248 | Date arithmetic; branch locals and one final sum expression match on third draft |
| 08229424 | 28 | Independently disassembled retained Thumb body; second draft MATCH |

Every accepted source passed check.py and a complete build printing `build/boktai3.gba: OK` before individual one-TU commits. Comparison, unsigned conversion and division aliases are supported by the existing exact libgcc unit and pinned runtime source. A prototype correction retains exact bytes. No gameplay names were introduced.

Two reviewed boundaries separate real Thumb bodies from preceding functions: 08229424 and 08232F1C. The latter standalone candidate MATCHes 76 bytes but is **not integrated**: the splitter retains its final eight-byte incbin and duplicates those bytes, shifting the ROM. Its safe candidate remains ignored in WIP. The boundary remains unmatched, preventing false credit for the predecessor. Shared gen was not edited by the parent.

Motion seed 08160EA4 was inherited from the previous session; eight fresh offset/index variants retained four register differences. Astra-low then tried ten variants and a bounded 90-second search without a match. The four-function family remains assembly. Time_CalculateSunriseSunset has a separate exact 124-byte candidate awaiting the next batch.

Batch 2: sub_0802CD04 matches its 460-byte body after replacing a retained explicit uninitialized-word vector read with field assignments and modeling bytes at 13C/144/145 and word140 as struct fields. This natural layout restores agbcc constant reuse. Its retained 64-byte tail contains independent Thumb bodies at0802CED0 (40bytes) and0802CEF8 (24bytes), each with own push/calls/pop-bx, independently exact natural C drafts. Both boundaries are explicit in build/progress metadata so the parent receives only460bytes credit. The first forwards incoming r1/r2/r3 and stacks p+0xC as fiftharg to08042588; the second forwards r1/r2 and passes p+0xC as fourtharg to08042610. No semantic names added.
