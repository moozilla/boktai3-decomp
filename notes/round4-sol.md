# Sol production round 4

Baseline `675db58aa333e31d3995f86e27b802db8b9ffa6f`; branch `codex/endurance-r4-sol`.

Eight net new matches, 1,912 emitted code bytes: four manually solved seeds, three manually corrected ports, one automatic port. Each source was absent at baseline. Exclusive range and reserved families/spans were respected. Checks and source snapshots remain in ignored `build/round4/`; unsuccessful C remains in `wip/round4/`.

Functions: 081C9DC0/081CA854 (172 bytes each), 081D10D8/081D116C (120 each), 08226B50/08226C44 (144 each), 081CAB20/081CAE88 (520 each).

The 520-byte pair initially compiled to 508 bytes because agbcc merged the defaults/callbacks in two inner switch tables. Moving the state counter increment into each outer case prevented that merge while the compiler still produced the original shared increment suffix. All 26 inner cases remain explicit.

The coordinate parser needs two separately escaping packed-coordinate locals. The callee 08226AE4 reads both r0 and r1; passing only one local deleted the second's writes. Passing both local pointers matches the word read/modify/write bitfields and the final r1=sp+8.

The 120-byte state setters require named zero/one temporaries and a named state value before the first two stores; this preserves callback-pointer loading and constant order. Automatic literal-immediate translation can accidentally rewrite unrelated field offsets: both 520-byte sources clear/set byte offset 7 despite swapped callback results 5 and 7. The auto-port mismatch exposed those two stores, then a manual correction matched.

Larger retained candidate 082038A8 is 436 versus 428 bytes, with register/spill differences; snapshot `build/round4/candidate-007.c`. Its 120-second permuter run aborted on a duplicate AST-node assertion. Vector-copy 08194858 retains a 244-byte same-size candidate with register allocation differences at `wip/round4/sub_08194858-best244.c`; a 90-second permuter trial scored 155 and saved no improved candidate. Callback 0819E1EC's 90-second trial improved score 1860 to 900; preserved under `build/permute/sub_0819E1EC/run-wrxy5y6s/output-900-1`. Other medium candidates remain uncommitted.

Three direct build-script runs print `build/boktai3.gba: OK` (logs fullbuild-01/02/03). No make or shared-gen regeneration. Separate per-TU commits follow complete builds. Parent handles combined emulator coverage.

Ordinary checks: 47 ({'mismatch': 34, 'error': 5, 'match': 8}); compiler-check time 23.33s. Matching stopped after 20.10 wall-clock minutes, then final validation and manifest preparation.
