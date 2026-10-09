# Existing-draft inventory (October 9, 2026)

This records the retained work subsequently preserved on the user-authorized
`codex/wip-drafts-2026-10-09` branch. The audit covers the main checkout's ignored WIP and
the six push20 worker/integration WIP trees, including earlier inherited drafts.
No new function targets or agents were started to produce this inventory.

The 435 C files are not 435 unfinished functions: many are alternate versions
or targets already merged. One C file could not be mapped to a known target by the source scan.
Source scanning identified 113 distinct known
unmatched targets. One representative per target and 68 additional distinct
existing variants were compiled against the baseline ELF. At this snapshot,
48 existing targets are exact (3,724 exact bytes) and 65 remain nonmatching.
The 48 exact candidates form the final reviewed integration batch. The other
65 targets remain nonmatching; no new targets were started.

## What survives a fresh clone

Accepted main source and committed notes survive. The separate
[WIP branch](https://github.com/moozilla/boktai3-decomp/tree/codex/wip-drafts-2026-10-09/drafts)
now also preserves 519 candidate C files, their manifest and selected-target
mapping. A fresh clone can recover all 65 unresolved selected candidates by
fetching that branch. Local check logs, permuter runs and binary outputs remain
ignored and are not in GitHub. The source-only local archive is retained too:
`build/existing-draft-sources_2026-10-09.tar.gz`.

```sh
git fetch origin codex/wip-drafts-2026-10-09
git switch --track origin/codex/wip-drafts-2026-10-09
```

Two exact drafts were overlooked before this sweep: 08232F1C (76 bytes) and
08248E70 (156 bytes, per-file -O0). A separate 308-byte alarm body remains
assembly; the added boundary prevents crediting it to the time-write entry. Most
newly finished clone candidates needed correct sizes, offsets, strides or
constants; literal-pool values and shifted immediates were not always repaired
by the earlier automatic port. Explicit uninitialized padding reads were
replaced with ordinary bitfield assignments before acceptance.

## What remains and why partial tracking could help

The near-matches contain substantial reusable source. The 1,060-byte motion
draft 08160EA4 differs at four instruction locations, but the bounded Astra
resume still failed. FDB4 has a 300-byte body with register/order differences;
its 788-byte inventory span includes a separate 488-byte retained tail. Do not
credit that tail to the draft. Other drafts have larger layout/control-flow
gaps, and similarity alone does not establish correct behavior.

Tracking candidate C in a separate directory excluded from the production
build would preserve algorithms, inferred layouts and failed hypotheses across
fresh clones. Store address, source hash, compiler/flags, mismatch evidence,
known safety issues and the next specific hypothesis. Keep these records
separate from exact code progress. Source chunks can guide family matching,
but adding the rest of a function can change register allocation and invalidate
a previously matching chunk. Raw instruction similarity is a diagnostic, not
a semantic correctness test.

The user authorized changing AGENTS.md to prohibit nonmatching C on `main`
and allow dedicated WIP branches. Candidate C is preserved outside `src/` on
the branch above. It is not accepted matching source and receives no exact
progress credit. Fuzzy/partial reporting remains a research question.

[Machine-readable inventory](../notes/existing-drafts_2026-10-09.csv) includes
the selected candidate, hash and compiler settings for every target. Exact rows point to accepted production source; unresolved rows retain their local origin and identify the preserved
branch path. Mismatch-line counts exclude identical stm/ldm writeback syntax.

| Target | Inventory span (bytes) | Differing instruction lines | Disposition |
|---|---:|---:|---|
| ObjGfx_LoadTiles | 144 | 46 | nonmatching; preserved on WIP branch |
| sub_080033FC | 76 | 0 | exact; accepted in combined batch |
| sub_08011AA4 | 44 | 0 | exact; accepted in combined batch |
| sub_08011C00 | 76 | 0 | exact; accepted in combined batch |
| sub_08016FE0 | 76 | 0 | exact; accepted in combined batch |
| sub_080182FC | 76 | 0 | exact; accepted in combined batch |
| sub_0801959C | 64 | 0 | exact; accepted in combined batch |
| sub_0801973C | 76 | 0 | exact; accepted in combined batch |
| sub_0802C620 | 88 | 0 | exact; accepted in combined batch |
| sub_0802D44C | 292 | 122 | nonmatching; preserved on WIP branch |
| sub_080379D8 | 52 | 0 | exact; accepted in combined batch |
| sub_0803A4D4 | 64 | 0 | exact; accepted in combined batch |
| sub_0803D5A8 | 64 | 0 | exact; accepted in combined batch |
| sub_0803F458 | 64 | 0 | exact; accepted in combined batch |
| sub_08042158 | 64 | 0 | exact; accepted in combined batch |
| sub_08043C24 | 40 | 0 | exact; accepted in combined batch |
| sub_08043CE0 | 76 | 0 | exact; accepted in combined batch |
| sub_080447AC | 40 | 0 | exact; accepted in combined batch |
| sub_08044814 | 64 | 0 | exact; accepted in combined batch |
| sub_08045704 | 64 | 0 | exact; accepted in combined batch |
| sub_08045744 | 52 | 0 | exact; accepted in combined batch |
| sub_08045CB0 | 68 | 0 | exact; accepted in combined batch |
| sub_08047258 | 76 | 0 | exact; accepted in combined batch |
| sub_080474A0 | 68 | 0 | exact; accepted in combined batch |
| sub_08048E0C | 224 | 76 | nonmatching; preserved on WIP branch |
| sub_0805E880 | 96 | 0 | exact; accepted in combined batch |
| sub_08069668 | 76 | 0 | exact; accepted in combined batch |
| sub_0806A5B4 | 76 | 0 | exact; accepted in combined batch |
| sub_08093330 | 64 | 0 | exact; accepted in combined batch |
| sub_08097BC0 | 164 | 3 | nonmatching; preserved on WIP branch |
| sub_080AB294 | 372 | 89 | nonmatching; preserved on WIP branch |
| sub_080C5314 | 148 | 0 | exact; accepted in combined batch |
| sub_080F4A0C | 64 | 0 | exact; accepted in combined batch |
| sub_080F6170 | 88 | 0 | exact; accepted in combined batch |
| sub_080F61C8 | 88 | 0 | exact; accepted in combined batch |
| sub_08105194 | 168 | 59 | nonmatching; preserved on WIP branch |
| sub_081134B0 | 132 | 55 | nonmatching; preserved on WIP branch |
| sub_0811FDB4 | 788 | 133 | nonmatching; preserved on WIP branch |
| sub_08125B68 | 84 | 1 | nonmatching; preserved on WIP branch |
| sub_08126000 | 116 | 46 | nonmatching; preserved on WIP branch |
| sub_0812642C | 316 | 133 | nonmatching; preserved on WIP branch |
| sub_081278E0 | 68 | 0 | exact; accepted in combined batch |
| sub_0812B7AC | 76 | 0 | exact; accepted in combined batch |
| sub_0812C314 | 156 | 0 | exact; accepted in combined batch |
| sub_08131B40 | 132 | 36 | nonmatching; preserved on WIP branch |
| sub_081388F4 | 48 | 0 | exact; accepted in combined batch |
| sub_081392DC | 48 | 0 | exact; accepted in combined batch |
| sub_0815C230 | 140 | 62 | nonmatching; preserved on WIP branch |
| sub_08160EA4 | 1060 | 4 | nonmatching; preserved on WIP branch |
| sub_081625C0 | 288 | 73 | nonmatching; preserved on WIP branch |
| sub_0816AC64 | 208 | 46 | nonmatching; preserved on WIP branch |
| sub_0816B47C | 96 | 0 | exact; accepted in combined batch |
| sub_0816F6B4 | 180 | 5 | nonmatching; preserved on WIP branch |
| sub_08171EFC | 128 | 45 | nonmatching; preserved on WIP branch |
| sub_08172AF4 | 220 | 4 | nonmatching; preserved on WIP branch |
| sub_08175AB0 | 36 | 0 | exact; accepted in combined batch |
| sub_08175AD4 | 36 | 0 | exact; accepted in combined batch |
| sub_08178544 | 32 | 0 | exact; accepted in combined batch |
| sub_08178FC8 | 460 | 7 | nonmatching; preserved on WIP branch |
| sub_081975C8 | 284 | 73 | nonmatching; preserved on WIP branch |
| sub_081A543C | 180 | 89 | nonmatching; preserved on WIP branch |
| sub_081A55D4 | 64 | 2 | nonmatching; preserved on WIP branch |
| sub_081BC820 | 352 | 67 | nonmatching; preserved on WIP branch |
| sub_081D5908 | 768 | 331 | nonmatching; preserved on WIP branch |
| sub_081DC470 | 424 | 9 | nonmatching; preserved on WIP branch |
| sub_081E013C | 696 | 71 | nonmatching; preserved on WIP branch |
| sub_081E8374 | 580 | 221 | nonmatching; preserved on WIP branch |
| sub_081F0E24 | 324 | 2 | nonmatching; preserved on WIP branch |
| sub_08201160 | 272 | 95 | nonmatching; preserved on WIP branch |
| sub_08201784 | 276 | 65 | nonmatching; preserved on WIP branch |
| sub_0820267C | 144 | 2 | nonmatching; preserved on WIP branch |
| sub_08210CB0 | 176 | 2 | nonmatching; preserved on WIP branch |
| sub_082132E8 | 212 | 2 | nonmatching; preserved on WIP branch |
| sub_08213D10 | 188 | 13 | nonmatching; preserved on WIP branch |
| sub_08213FD8 | 364 | 102 | nonmatching; preserved on WIP branch |
| sub_082157EC | 196 | 92 | nonmatching; preserved on WIP branch |
| sub_082158B0 | 180 | 48 | nonmatching; preserved on WIP branch |
| sub_082285C4 | 184 | 6 | nonmatching; preserved on WIP branch |
| sub_08229180 | 216 | 98 | nonmatching; preserved on WIP branch |
| sub_08229440 | 40 | 15 | nonmatching; preserved on WIP branch |
| sub_0822AF78 | 88 | 30 | nonmatching; preserved on WIP branch |
| sub_0822AFE8 | 88 | 5 | nonmatching; preserved on WIP branch |
| sub_0822B058 | 88 | 5 | nonmatching; preserved on WIP branch |
| sub_0822B5FC | 68 | 27 | nonmatching; preserved on WIP branch |
| sub_0822B6C8 | 192 | 84 | nonmatching; preserved on WIP branch |
| sub_0822B7E4 | 168 | 81 | nonmatching; preserved on WIP branch |
| sub_0822B968 | 124 | 41 | nonmatching; preserved on WIP branch |
| sub_0822BBD8 | 148 | 0 | exact; accepted in combined batch |
| sub_0822BD58 | 96 | 9 | nonmatching; preserved on WIP branch |
| sub_0822CAA0 | 104 | 50 | nonmatching; preserved on WIP branch |
| sub_0822CE54 | 88 | 0 | exact; accepted in combined batch |
| sub_0822D080 | 72 | 5 | nonmatching; preserved on WIP branch |
| sub_0822EDF8 | 112 | 2 | nonmatching; preserved on WIP branch |
| sub_0822EE68 | 220 | 75 | nonmatching; preserved on WIP branch |
| sub_08231EF8 | 228 | 26 | nonmatching; preserved on WIP branch |
| sub_08232F1C | 76 | 0 | exact; accepted in combined batch |
| sub_08234354 | 76 | 0 | exact; accepted in combined batch |
| sub_08234770 | 88 | 28 | nonmatching; preserved on WIP branch |
| sub_08235000 | 216 | 0 | exact; accepted in combined batch |
| sub_082359CC | 144 | 32 | nonmatching; preserved on WIP branch |
| sub_08235B34 | 252 | 18 | nonmatching; preserved on WIP branch |
| sub_08235D68 | 72 | 0 | exact; accepted in combined batch |
| sub_0823652C | 112 | 14 | nonmatching; preserved on WIP branch |
| sub_08237E7C | 76 | 0 | exact; accepted in combined batch |
| sub_08239150 | 88 | 0 | exact; accepted in combined batch |
| sub_08239CDC | 92 | 27 | nonmatching; preserved on WIP branch |
| sub_0823A2E4 | 164 | 34 | nonmatching; preserved on WIP branch |
| sub_0823ADB4 | 168 | 42 | nonmatching; preserved on WIP branch |
| sub_0823AE5C | 232 | 41 | nonmatching; preserved on WIP branch |
| sub_0823E704 | 268 | 123 | nonmatching; preserved on WIP branch |
| sub_0823EDDC | 164 | 39 | nonmatching; preserved on WIP branch |
| sub_08243B54 | 188 | 43 | nonmatching; preserved on WIP branch |
| sub_08248E70 | 156 | 0 | exact; accepted in combined batch |


The inventory's function count does not come from one-file-per-function source
splitting: it records binary entry addresses. After the RTC boundary, there
are 11,081 known starts versus 11,025 generated starts (56 explicit additions).
The pre-rescue inventory's median span was 100 bytes, with 3,940 spans at most
64 bytes. This describes many short routines; it does not establish that GBA
games generally contain more functions than other platforms. Our progress
report maps each function to one unit, whereas objdiff units normally represent
translation units, so compare actual function totals and code bytes rather
than unit totals across projects. See tools/progress.py and docs/PROGRESS_CI.md.
