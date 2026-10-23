# Campaign worker sol11

Tags: #matching #actors

- 080DE6E0: skipped after initial reconstruction (49.4% scratch). Gait-frame sound lookup and particle emission understood; packed vector initialization, RNG lifetimes and effect argument allocation remain structurally different. Ghidra export unavailable. Candidate retained locally for a later pass.
- 080DEA7C: skipped; original draft remains 99.4%. Opaque shared offset fixed the subtract-four instruction, but shifted nine later scratch-register choices. Pinning sine index introduced extra moves; retained original draft.
- 080DED74: skipped; retained 97.9% draft. Explicit r5 offset pin disrupted prior allocation; a bounded single-process permuter search saved no improvement.
- 080DEF78: drafted 66.8%. Recovered terrain slope adjustment, directional neighbor probe, randomized turn preference and route validation. Signed truncation helper preserves direction branch shape.
- 080DF6EC: skipped, scratch 57.7%. Decoded byte-only callback: inventory selection and consumption, health-proportional damage, target bearing/distance refresh. Redundant reset stores and packed-vector lifetimes differ.
- 080DF908: skipped, scratch 61.9%. Reconstructed random radial trail velocity and particle kind selection. Original retains an unused hardware-address value and copy; pointer escape repaired halfword stores, but register/literal lifetimes remain different.
- 080DFC34: skipped after full control-flow/relative review. Eight-direction turning dispatch expands animation helpers to 816 instructions; closest matched relative covers 37%. Sequence/keyframe selection, flip flags and duration restoration identified, but no cohesive candidate retained.
- 080E41F8: matched, 312 bytes, register-pinned. Kept collider arguments in source load order and separate zero lifetimes; depin reduced sixteen hints to five.
- 080E4330: matched, 296 bytes, register-pinned. Ported leap setup with trail fields; materializing the context mask before computing its field fixed the final ordering difference. Depin reduced six hints to five.
- 080E4B34: skipped, retained 87.7%. Pointer-local elimination worsened alignment; recovery/turn inline has forty structural instruction differences.
- 080E6BE4: skipped, retained 84.0%. Context pin plus shared flag-pointer lifetime increased structural differences; reverted.

## Wrap-up batch

The wrap-up marker arrived before the Glove family began. Exact gain is 0.0252 percentage points of code (608 bytes), all in the Register-pinned category. Both matched functions already have canonical names in the symbol registry, so their existing linkage names were retained. The facing-route draft is excluded from exact progress. Remaining assigned functions were not started.

Validation: combined batch build printed `build/boktai3.gba: OK`.

Batch result: 2 matched, 1 drafted, 8 skipped; resume at 081172C4.

## Continuation session h2: carried Glove drafts

- 081172C4: drafted 74.2%. Launch ascent/slowdown and stopping cue reconstructed; motion pointer allocation and terrain-response ordering remain different.
- 081175F8: drafted 70.6%. Launch ascent/slowdown and callback transition reconstructed; common movement and scratch-register lifetimes remain different.

Validation: carried draft batch rebased onto current main; combined build printed `build/boktai3.gba: OK`. Both functions remain guarded and earn no exact progress.
