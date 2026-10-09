# Sol production round 3

Baseline `c15f99180ccf7e118c641b9280180110a294ed02`; branch `codex/endurance-r3-sol`.

40 net new functions, 6,736 emitted bytes. 16 independently adapted seeds, 16 manually translated family/template variants and eight automatic ports. No baseline rewrites are committed. One already matched 08171410 recheck was restored before commit and excluded from progress.

71 ordinary checks: 42 successful checks (40 new functions plus one repeated verification and the excluded baseline recheck), 29 mismatches, no check-tool errors. Maximum checks on any target: 6. Matching elapsed 24.81 minutes, including build waits. Bounded permuter logs are retained separately; the 08190E8C trial solved its seed, while 08203D6C reached score 5 before its table-entry alignment was corrected manually. The bitfield setup trial aborted with an AST duplicate-node assertion; a manual struct-field correction solved the seed. Two angle-family trials and the larger indexed-family trials did not improve; manual ordinary-C changes solved those families afterward.

Full builds 1 through 8 each printed `build/boktai3.gba: OK`. No make or generation commands were invoked. Parent handles the combined emulator scenarios per its final round instruction. All source additions are separate per-function commits in batches of five after a full build.

Useful patterns:

- Taking the address of a local base pointer before adding an index changes tree expansion order without forced registers. Taking the address of the final indexed pointer instead unnecessarily prolonged another register's lifetime in the larger functions.
- Taking the address of an explicit local mask preserved its load before the object-pointer calculation; this solved 081D84FC and both variants after bounded searches stalled.
- Plain agbcc structs containing two bytes still have size four. A packed two-byte table-entry struct preserves the RNG index scale and load order (08203D6C/4EB8).
- Assign separate explicit switch cases even when their result values are identical. Grouping defaults collapsed 081CBCBC's 26-entry table; separate assignments reproduced it. The two 484-byte 081DF32C/694 state mappings matched directly from their explicit case tables.
- Function argument expressions can preserve store/load sequencing better than hoisted scalar locals. This solved 081708E4's two trailing halfword parameters.

Retained follow-ups: `wip/round3/sub_0817026C.c` is the best 204-byte timer candidate versus a 208-byte original. `build/round3/candidate-030.c` preserves the shorter 081A543C transform candidate; the current WIP is a later inferior experiment. Old round2 WIP and logs remain preserved. Reserved root animation functions and the new MGS/GCL spans were excluded; none were edited.

Detailed timestamps, candidate snapshots, transcripts, emitted sizes and automatic ports are under ignored `build/round3/`. Exact ordered matching commits:

- `66e8946a9d7377a5a06315363a3c2eb269bd2312` Match sub_081FE6E0
- `9601f6862b42c25a4b7f5a720d8b8f3eb0084057` Match sub_082116A8
- `81cab0e3a3a8b6a55f831f08de8fb331225e6f1b` Match sub_081FF02C
- `0c33d2aba027d95a8e176e20de70ee5343180a24` Match sub_082010EC
- `e8dc6f33a8990e2c4e9d3085cc8258e1a8ee5137` Match sub_0820A420
- `d96855f6eb766d5cb54c1bc40f56f0f4c9097e0d` Match sub_08190E8C
- `a23bd83f8f17a36673720c429a1dfe3673b3c1c9` Match sub_081913EC
- `1a986667478d2a0d66a275dc808a3cc50126794e` Match sub_08203A54
- `439b198acebe52f00e818a7612cd39a4d206dcf9` Match sub_0822D0F0
- `de3d2bb91a3614e9aea2e749a4d795dce1a4ef83` Match sub_0822E18C
- `a09f2392da69b96fb8c78e3013ac2ec2c204c543` Match sub_0822EC10
- `035836d08dba25860d3592bd71f58ab86f93de4e` Match sub_08210B50
- `0e2a2e2a314b1b045790a58af842c344cc75247c` Match sub_08204B3C
- `0c8b01dce333c0e74dd7abeade7ffc4f87b9bb45` Match sub_0820DDC4
- `027d35ae25b2114686d4524d15b3ff0ad287b893` Match sub_082083A0
- `14b0860070870d826fae8c8d1d9022b610164139` Match sub_082130D4
- `b9a3a09730372957b5a2e728d18f159568d1e2d1` Match sub_082068A4
- `893aeddd470507b1994a4d70dd42c4733aa0b6b6` Match sub_0820AF30
- `d1fa03cf05bc571e181f2d2f3f76d333db51a3c0` Match sub_08203D6C
- `16e9633975e80e08afa8814918ba260df973a9f0` Match sub_08204EB8
- `c30e0cd664acc604d99109f1819804e009dea455` Match sub_08171C44
- `536028af2b3da88ee9c3cebd80ab6d466fea86ba` Match sub_08173FB4
- `e792852baf2954d2f6ae3986f739dc5af18df732` Match sub_08190DCC
- `256a855f5ba06d47bfaf0c223c9d90b07797823b` Match sub_08190FA8
- `39ed3ad79a0a8ed9a98c0360939cd4d756b728c7` Match sub_0819132C
- `a5d66c29254361f3a11d9a612dc26cb2c43e37a9` Match sub_08191278
- `6fa3e732da1f525d4ad246c0a30908a3ea84df19` Match sub_08190EEC
- `e6fa26cd6671f11c7f03aca22142274b571ac8ea` Match sub_081D84FC
- `6d872a628ffed97592178980fd5714adf4093c3f` Match sub_081D8854
- `3e73f66d8ab2d47758d6202dae016332e530a163` Match sub_081D8B14
- `9ecf066dbe927a6496b86581980c420775142a33` Match sub_0819144C
- `9b75919b95d043291c16ce9f004e20f1d638ebe5` Match sub_08171524
- `71648d5d8d111e6213fa104878fdd7e3dc9b3a56` Match sub_08173B24
- `4945d2992f66f481a5b1e5a5a1d97f34b39f32c0` Match sub_0817033C
- `2d151d73b03f9b0ed7295c9edbad740a438f1d90` Match sub_08170F70
- `2dfcea3b007ccd4d24b36556a0f4e5cac5148bef` Match sub_081DF32C
- `4a4f7f1ff174418d8ed35db7fb69beddbaa5029d` Match sub_081DF694
- `b1522d60ab71704cd7b5373fecbc404d54188ad8` Match sub_081708E4
- `13539fda833d5c2895c2eed793ea7ea8ffb05a9e` Match sub_081CBCBC
- `4d0cb80391b390eeb6cbccad1e4b9e8cc12d84a4` Match sub_08178518
