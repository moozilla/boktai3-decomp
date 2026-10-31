# Finish-player integrator handoff

Tags: #matching #player #compiler #structs #cleanup #data #studio

This bundle preserves integrator-ready work that previously lived only in ignored
scratch directories. The current code and guarded drafts are already committed
on `codex/finish-player`; start with [the batch note](../finish-player_2026-10-30.md).
The checkpoint before this bundle is `4d63ef54a`.

## Current source

Nine of eleven assigned functions are exact. The remaining guarded source is
`src/fn/sub_0814B1F8.c` (99.6% fuzzy) and `src/fn/sub_08200C94.c` (96.1%).
Normal builds retain their exact assembly fallbacks. The matching reports here
archive the closest candidates, compiler decisions and next source questions.
Their original scratch-relative references are provenance, not required inputs;
resume from the committed C. Diagnostic knobs must not enter production code.
The clone-pool committed source includes the final typed callback and corrected
two-halfword operation view; earlier experimental reports may show older views.

## Prepared cleanup patches

These patches are unapplied. Finish the remaining matches, then apply each
reviewed cleanup separately, format the affected C and rerun its explicit-extent
stock check. All patch paths are repository-relative and use `git apply -p1`.

| Patch under cleanup/ | Scope | Prior validation |
|---|---|---|
| `08146EAC-depin.patch` | Ordinary playback: six pins and five barriers removed; separate game-state pin remains | Exact 2,188 bytes |
| `0814B078-depin.patch` | One freely removable pin | Exact 360 bytes |
| `0814C248-depin.patch` | Three freely removable pins; barriers remain | Exact 336 bytes |
| `0814D9C4-depin.patch` | One freely removable pin | Exact 780 bytes |
| `data-studio.patch` | Seven Player callback CSVs, manifest browsing metadata, generic Studio callback browser and tests | 14 focused tests; 18 tables / 1,080 bytes round-trip |
| `data-roundtrip.patch` | Optional seven new Player data-evidence rows | New 744 bytes independently round-trip |

`depinning-validation.json` records exact reconstructed-source SHA256 values and
stock check results. The two data validation JSON files preserve byte ranges,
null/relocation counts, syntax, dispatcher and symbolizer checks. The Studio
preview used a minimal read-only fixture, not the complete live Studio index.

`0814DF58-review-only.patch` preserves a stock-exact 132-byte expanded-macro
candidate proving two free MODE_OFF barriers. It is evidence, not the proposed
final implementation: specialize the existing read helper instead of landing
expanded repeated bodies. Its remaining enabled-field barriers still matter.

The ROM-order pin scan covered 77 concession-bearing functions in this section;
other tested direct removals changed code. No whole translation unit becomes
concession-free from the four proposed cleanup patches alone.

Shared-record work remains an inventory, not an applied patch. The archived
`cleanup/record-inventory.md`, `initial-inventory.md`, `pinned-rom-order.tsv`,
`playback-family.tsv` and `direct-rom-references.json` preserve that audit.
Original scratch-relative paths in these reports describe the original work. The seven orbit
functions share a 0x3C particle and 0x4C8 task layout. Clone and playback-family
records are the next shared-header candidates.

Do not split the high-ROM 90-word callback region into independently bounded
arrays: observed indices cross apparent labels and the manifest lacks alias
views. `08610E24` is scalar data, not a callback table.

After review, run the complete combined ROM build once and require
`build/boktai3.gba: OK`, then regenerate both progress files for batch integration.
No combined build, merge, push or PR was performed by this worker.
