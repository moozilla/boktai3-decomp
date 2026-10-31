<!-- Archived worker evidence. Original scratch-relative paths below are provenance; current guarded source is committed under src/fn/. See ../README.md. -->

# B1F8 fresh table-capture review

Tags: #player #matching #compiler

Scratch-only assistance; no tracked edits or commits. The active function owner is the sibling Player motion worker. Starting source: round4/natural_data_late_frame.c (30ROM/24draft,shape26,4164B; ROM4176B).

## Two recovered ordinary source properties

1. Remove the manually captured `const s16 *sine = gUnk_086149C4` in PlayerMotion_SetPolar and use a direct `gUnk_086149C4[angle &255]` Z lookup. This lets PRE share the existing function-global table holder and place all four required high-register captures immediately after the X multiply. `polar_direct_table.c` becomes20/20,shape12,4176B. No high-copy hunk remains. Merely commuting the final helper multiplication is unchanged30/24.
2. The newly shared global table exposes plain PLAYER_START_ANIM macro owner's unnecessary retained alias in later cases. Apply the already successful phase0 ownership pattern to that plain macro: separate branch-local channel/data arguments, then a freshly captured flagChannel after the join. `direct_table_fresh_macro_owner.c` becomes13ROM/11draft,shape4,4172B. OWNER and SHARED macro variants are retained for their deliberate later uses. The main worker has an independent phase2 direct-result edit to stack onto this.

## Phase5 reload proof

Changing phase5 itself to the new plain macro plus direct Anim_Step(&p->secondaryAnimation,p->animationData) is byte-identical to13/11; no further owner change is needed there.

Every remaining phase5/later register row is one reload rotation. On `direct_table_fresh_phase5.c`, `AGBCC_SPILL=137:1` gives4ROM/2draft,shape4. The only remaining hunks then are the phase2 result-copy/PRE-table order and phase3 posZ stack-address recomputation (plus dependent pool padding). Proof also saved for the smaller-source macro-owner variant in `phase5-spill-proof.txt`.

why4654 identifies phase5's first heading-offset literal930 as reload137. Scan starts afterr3 and takesr5, but ROMtakesr1. The pool in order isr0,r1,r2,r3,r5. The ROM's missing phase3 posZ address reload may supply the extra rotation; main worker is investigating that source boundary.

No compiler knob appears in a candidate source. This audit has no independent exact candidate yet; final ordinary/stock verification belongs to the merged function owner.

## Direct aggregate owner fixes phase3 stack load

`pos_six_case.c` replaces the function-level union's `pos.value` owner with an actual case-local XYZ struct. It gives9/9,shape0,4176B: phase3 posZ stack-base recomputation and the padding are exact. Further tests corrected the extent interpretation: an eight-byte distinct struct, a plain actual ContactPosition local (`pos_contact_unaligned.c`), or a union with a separate XYZ components member also give9/9. A case-local original union still gives12/10. Thus actual aggregate/member ownership, not six-byte storage extent or alignment, decides the stack address reuse. The plain ContactPosition local is the smallest clean source edit; typed XYZ helper is also byte-identical.

Full canonical/follow/PRE probe on9/9:163 compiles (63 canonical-register choices,74 CSE jump follows,26 PRE replacements), none changes the count.

## Final phase5 register reservation

On `pos_contact_unaligned.c`, first phase5 heading constant930 is reload choice137. Prior choices134/135/136 all match physically and tookr1/r2/r3; choice137 scans afterr3 and choosesr5. ROMneedsr1, implying an extra/missing rotation or r5 unavailable at this point. Plain macro, OWNER macro, fresh flag owner with shared data, and direct branch argument forms stay9/9. Shared flags plus fresh direct AnimStep is worse25/27; hoisted channel13/11.

Hoisting only `data=p->animationData` before heading gives `phase5_hoist_data.c`,4ROM/2draft,shape6. Every register choice is now correct through the tail; only two hoisted address ops vs four branch-local address ops differ. animationData is embedded atp+0x6c. Reserving itsr5 before heading blocksr5 for choice137. The remaining source task is a legitimate data owner/copy relationship that reservesr5 while retaining branch-local address operations, or a prior reload inheritance difference. No compiler knobs or identity gadgets appear in candidates. Ownership of the remaining9/9 frontier was handed back to the primary B1F8 worker.

## Final reload-history review, stopped at quota wrap-up

Root requested bounded wrap-up when the user set a20% remaining-quota reserve. The final quota read was23% remaining (77% used). No new experiments are pending. The guarded source is untouched and no complete ordinary exact candidate is claimed. `last-clean9.c` preserves the current9/9shape0,4176B ordinary source from round4/plain_pos_simple_frame.c. Its independent stock result is10 differing bytes,99.6%, explicit target4176B, artifacts build/check/run-fgdpxgyp; stock function SHA2569c61cfb3efa701970712d342e8b9b252bc2b95b551103ffc25c15fd89d0917d0.

### What the trace proves

* r5 first enters the spill set at insn2154, phase2 X signed table load: `(set r1(sign_extend MEMHI r0)) (clobber r5)`. Physical `movs r5,#0; ldrsh r1,[r0,r5]` exactly matches ROM. A four-register spill pool therefore does not explain the difference.
* inherit-review/reload97-137.csv records each choice, source line, value, final ROM/draft instruction and survival through jump2/mach. Choices134/135/136, UIDs4535/4548/4603, all physically survive and match.
* Full164-case SPILL sweep k97..137,d1..4: only137:1 and137:2 MATCH. No earlier existing choice improves9. Results: inherit-review/spill97-137.json.
* A missing choice can still occur in the phase4 negative-flags block, which jump2 deletes. Its flags temp1930 currently uses native0 and mask1931 native1, with no reload choice. This is the actual new source frontier below.

### Private compiler and ignored-hook correction

Both the production scope binary and the earlier Burst-PREF private binary silently ignore AGBCC_NOINHERIT: the hook exists in the checked-in patch but was not built. `diag-reload/` is an isolated copy of the Burst private compiler with that hook added; no production file changed. A positive full compile reports `NOINHERIT matched uid=4645`.295 UID probes across choices97–137 change nothing. A full compile with inheritance disabled for every instruction is also byte-identical9/9. Thus choose_reload_regs inheritance is ruled out. Ignore `results-ignored-hook.json`; it explicitly preserves the earlier unsupported-knob run without counting it as evidence.

The renamed draft also falls back from scope's fork server because source_group omits its macro-defined old assembler label. Private drivers add sub_0814B1F8 to source_group only in that Python process. This restores fast serving. Private no-knob, stock and g/no-g function bytes are identical, recorded in identity.json/compiler-identity.json. Production tools were not edited.

### Native flags eviction proof

The private compiler adds AGBCC_EVICT, which frees a specified pseudo before ordinary reload and prevents retry_global_alloc assigning it back. This is a diagnostic, not a candidate option. Source/patch and compiler hash are preserved in diag-reload/diagnostic.patch and inherit-review/compiler-identity.json.

AGBCC_EVICT=1930 (phase4 negative flags temp) fixes every one of the9 register rows. ONLY remaining differences are stack prologue/epilogue2C→30 for a new4B spill slot. Positive flags1927 gives the same proof. `inherit-review/evict1930.diff.txt` preserves the complete result. This proves a hidden outputreload0 in the later-deleted flags block supplies the missing cursor history.

Simple local raw-word unions/structs, full32-bit bitfield, arrays1/2, const/named input/result, builtin memcpy and explicit-address versions all compile identically9. BLK containers (packed/unpacked12B, union raw+bytearray5/6, register variants) retain a dead stack store, prevent tail cross-merging and change the frame; none is selected. The primary worker tested31/30/24-bit field views: they fix the cursor but retain mask/merge instructions; single-bit local views fold too early and remain9.

### Separate input/result and MEM equivalence

`phase4_negative_const_input.c` has distinct input1930/result1932 through combine at AND UID4626. regmove then reties result to input, leaving two sets and preventing REG_EQUIV to the real animation flags memory.

AGBCC_NOTIE=4626 preserves both single-set pseudos. local-alloc gives BOTH a valid REG_EQUIV to the actual flags memory. Evict result1932 alone remains9: outputreload shares native input0 in pass0 and does not advance the cursor. Evict input1930 and result1932 together fixes ALL9 without frame growth, but reload now moves LDR after the already-native mask setup, so the negative block fails to cross-merge:3ROM/5draft,shape8. Evict mask1931 as well restores load/mask order but produces the wrong rotation (20/20shape4). These are causal diagnostics only. The primary worker's last bounded idea was a real READ_CONTROL load macro boundary (do-while0 around the load only) to prevent regmove retie while leaving the AND result outside loop-weighted REG_N_SETS. No new result from that pending owner experiment is claimed here.
