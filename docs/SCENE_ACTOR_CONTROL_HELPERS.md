# Scene actor control helpers, 08083AEC–08084280

The whole1940-byte subsection was reviewed before matching. Existing exact credit is420B:83AEC164,83BFC60,83E1828,83E7040,84100128. New potential is1520B. Five new entries now match476B, all register-pinned:83B90108,83E3460,hidden83F3C32,83E98164,84210112; this adds about0.0197percentage points to whole-ROM matching. No old exact leaf earns additional credit.

Actual boundaries are83AEC/83B90/83BFC/83C38/83E18/83E34/83E70/83E98/83F3C/83F5C/84044/84100/84180/84210/84280. The previous83E98 metadata span contains three separate functions:164B,32B,232B. Split those boundaries during integration without changing the denominator.

83B90 reads context3D0's unsigned phase3E0, divides by128 through the signed truncating helper shape, takes absolute value and clamps to7. It returns an integer entry from the eight-word ROM table08605C3C, adding6 when context flags3D4 bit8 is set. ROM words are2,3,4,6,7,9,10,12; this is a numeric preset ID, not a pointer. The preset interpretation is tentative; the arithmetic is verified.

83BFC copies the full8-byte actor position5C and destination argument into context vectors41C/424 and zeroes3E0. 83C38 copies both vectors, clears their Y components, forms a signed-halfword difference, zeroes another vector with CpuSet, and evaluates three cubic weighted coordinates into context3FC. It uses phase3E0 and signed context3E4, whose role is mode-dependent (other callbacks use that field for sway). Phase>1023 resets to0 and returns1. Otherwise it advances the explicit RNG once, adds256 plus one table byte, clamps the stored unsigned halfword to1024, and returns0. Parameter ranges needed to exclude signed multiplication overflow remain a draft precondition; do not credit an unchecked polynomial rewrite.

83E18 selects default profile category18/mode0. 83E34 sets actor flags2D0 bit100000, clears context flags3D4 bits0..2, and clears colliderB188 flags bit4. 83E70 maps an incoming halfword angle into a symmetric0..4 index and a side flag.

83E98 starts position interpolation through83BFC/83C38, writes actor state0/phase0/behavior1, counter0 and2B1=1, derives3CD through Atan2 of the signed X/Z destination difference, and zeroes context vector42C. Hidden83F3C clears actor flags2D4 bit100 and statusEB bit8. Hidden83F5C probes terrain at position5C plus vector2EC; when actor ground2BE is below the terrain height, it negates vector2EC X/Z and clears motion64 X/Z. Its map bounds fallback and two slope shapes agree with adjacent terrain helpers.

84044 clears context phase3E2, zeroes actor vector2EC, clears context low flags, sets statusEB8,actor2D0100000,and2D4100; writes state0/phase2/behavior0 and restarts interpolation toward player-slot0 position30 after enabling shadow320. 84100 consumes statusEB2, compares actor/player vertical positions within512, requires unsigned wordsF0<F8, then sets2D680/statusEB10 and returns1 (otherwise0). 84180 sets statusEB8 and, if actor2D4100 is clear while context bit2 is set, sets that actor bit and state0/0/3. 84210 conditionally hides the drawable, clears actor's own word3D4 bits60 or calls8484C, always calls838AC and returns word1. The actor's own3D4 word is not asserted identical to its separate context's3D4 word.

Old83AEC contains two explicit uninitialized scalar RMWs in its zero-vector construction. A private defined repair uses three signed16 bitfields (ContactPackedVector) and assigns all named XYZ components, leaving only padding unspecified. This preserves all164 bytes exactly as ordinary C; parent will port it onto canonical typed collider calls. The original two-field H layout narrows zero assignment to STRH and is unsuitable for the repair.

At the20%-remaining cutoff,84210 had a full112-byte exact proof and standard hint reduction19→2;83E98 had full164-byte proof and21→8. They were frozen without further trials after the cutoff. The83B90 integer-return QC changed only word/pointer types after ROM-table review; parent must rerun its108-byte proof during final combined validation. Remaining helper bodies stay assembly/private drafts.

## Resumed helper closure

The completed82184 focused permuter run saved no candidates; its baseline remained30. Broad-search outputs contained only the already incorporated valid zero-store simplification. The repeated anonymous-declaration mutation assertions produced no exact evidence.

The resumed pass closes84180(144B),84044(188B),and hidden83F5C(232B), all full-extent exact after standard hint reduction to7,13,and9 hints respectively. This adds564B,about0.0233percentage points; the subsection now contains1040B of new exact C, all register-pinned, with83C38(480B) still assembly. The existing420B are excluded from new progress.

84044 uses the canonical player.h pointer array and its position30 field. The terrain probe uses signed arithmetic for slope-adjusted height and compares against the zero-extended ground2BE value; negative heights therefore do not cause reflection. Its XYZ sums remain within32-bit arithmetic before halfword narrowing, and no C scalar reads the temporary vector padding.
