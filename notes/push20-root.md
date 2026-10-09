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


Batch 2 adds 12 parent functions / 2,640 emitted bytes, including the 124-byte wrapper, 1,624-byte Time_CalculateSunriseSunsetCore, 460-byte script initializer, four audio wrappers (336 bytes), and five retained helpers (96 bytes). All new gameplay routines retain their existing names.

The core uses the observed 12-double/four-integer context layout. Six drafts recovered the full 1,624-byte body: preserve midpoint and transit locals around the initial calls; compare the two final iteration results; put comparison results before assigning rise/set outputs; keep rounded minutes separate from truncation; reuse the final digit local. Its return is zero. The wrapper declaration now agrees with the core's u32 return and remains an exact 124-byte match. No external astronomical implementation was copied.

sub_0802CD04 matches its 460-byte body after replacing an inherited explicit uninitialized-word read with vector field assignments and modeling bytes at 13C/144/145 and word140 as struct fields. Its retained 64-byte tail has independent Thumb functions at 0802CED0 (40 bytes) and 0802CEF8 (24 bytes), each with its own prologue/calls/return and exact natural C. Explicit build/progress boundaries prevent crediting the tail to the parent. The first forwards incoming r1/r2/r3 and passes p+0xC as the fifth argument to 08042588; the second forwards r1/r2 and passes p+0xC as the fourth argument to 08042610.

sub_08227ECC ends at 08227F3C after its return and RAM literal. Retained context helpers start at 08227F3C (16 bytes),08227F4C (12 bytes), and08227F58 (4 bytes). The words at 08227F48 and08227F54 are literals, not entry points; parent disassembly corrected a worker's initial address estimate before accepting metadata. Three standalone C checks match, and explicit boundaries keep 27ECC credit at 112 bytes. The setter's normal inline store helper preserves value-before-pointer code generation; the getter returns byte9, and the final function returns zero.

Inherited RGB48E0C drafts checked220/208 bytes without improving extraction/register allocation; no source accepted. Two fresh ordinary inline division variants of the high worker's 285C4 seed retained its six instruction-order differences; no source accepted. Safe candidates remain ignored in WIP. Three adjacent audio fade wrappers also remain WIP after three checks each; argument/global-pointer allocation differs.

The continuation after the usage check adds eight fresh root matches /912 emitted bytes: 08233924(164),082339C8(84),08233A1C(132),08233AA0(96),082346EC(132),08239C80(92),0823ACD0(72),0823B278(140). All matched within one or two drafts, and the batched complete build printed OK before one-TU commits. Fresh call-heavy initializers and real structs yielded more than the retained allocation misses. A paired-halfword entry array preserves repeated even offset constants in 39C80; temporary loads precede destination address preparation in ACD0; u32 stack argument d, truncated only at its store, fixes AA0; three initialized 16-bit vector fields retain ordinary compiler padding in the constructor family. No explicit uninitialized-word reads are used.

Unaccepted fresh drafts A2E4 (two checks), D080 (three checks), ADB4 (one check) remain WIP. No permuter or new tool regression suite ran. Review caught an unsequenced assignment/read of zero in a new mid-worker call and two earlier integrated calls; the worker was tasked with exact safe repairs, using literal zero for other arguments rather than reading the assigned variable in the same argument list. These repairs do not add matching-byte credit.

The progress-span audit of the combined batch found 136 retained bytes after 0822C0CC's 68-byte body. Read-only Thumb disassembly established four independent entries: C110 reset body and literals end C154; C154 conditional getter and literals end C180; C180/C18C halfword setters each end after a 12-byte body/literal. All four standalone first drafts MATCH (68/44/12/12). Explicit build and progress boundaries isolate the parent and account for the four independently decompiled entries. Shared generated assembly stays untouched. Final root continuation gain:12functions/1,048 emitted bytes.
