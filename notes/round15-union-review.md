# Round 15 — union ownership review

Tags: #matching #structs #phase3 #round15

Updated 2026-11-17. This is the next tier after the reported dead-code / proved-zero
and volatile investigations. The term `local-union` is the scanner's label; the
actual inventory also includes unions inside record declarations outside functions.

## Inventory and decision rule

The immutable inventory was captured after the result-panel recovery removed one
of the round's original 131 reports. It contains **130 sites: 96 inside functions
and 34 in record/file declarations**. Its source hashes, declarations, actual uses,
subsystem/ROM context and prior direct-split results are preserved in the portable
research archive. Resolutions below do not overwrite that baseline.

| Review category | Baseline sites | What the evidence currently says |
|---|---:|---|
| Sequential unrelated roles | 32 | A pointer, index, callback, flag word or other unrelated value reuses the same storage. These need real source/dataflow recovery; shared storage is not justified merely by nonoverlapping lifetimes. |
| RNG address wrappers | 26 | Single-member table unions or byte/halfword reconstruction of a fixed RAM address. No serialized runtime address representation has been established. Existing comments often explicitly describe hoisting/allocation effects. |
| Views of the same vector | 22 | Coordinate halfwords and packed words refer to the same value. That is a legitimate representation category, but the original union spelling or hand-masked operation still needs evidence. Prefer actual native vector owners/operations where recoverable. |
| Scalar views / narrowing | 16 | Includes same-value narrowing as well as one-member mask wrappers and fabricated pointer projections. These require individual decisions; this category is not an exemption. |
| Record ownership candidates | 13 | Several unions overlay a native owner with padded partial views, or invent an array extent/stride. Initialization, consumers and allocation sizes can establish the actual record. |
| Record representations | 21 | Potentially real paired views, tables or variant storage. Original library source or independent producers/consumers are required before adding a justification. |
| **Total** | **130** | A classification queue, not 130 proven concessions or 130 legitimate representations. |

Fourteen prior round-14 direct-split misses are linked in the inventory and were
not blindly repeated. A byte match after an arbitrary declaration permutation is
not evidence that the original used the resulting source.

## Accepted resolutions

| Site | Resolution | Evidence and acceptance |
|---|---|---|
| Result panel (`0803B40C`) | Separate the sequence object and priority byte; remove the reused aggregate | `409ad36bb`; full 684-byte unit exact. This was already removed before the 130-site inventory. Three dead arms and one zero also disappear; resource-copy guards remain. |
| RegionMenu palette | Replace colors/pulseState overlay with the actual sixteen-color array and color 12 | `edf6056d6`; native pulse routine writes one u16 color while phase is a separate byte. Menu size 0x5B8, palette +0x584 and state offsets are checked; complete unit and dependent match. |
| Vulcan cannon, two declarations | Use one DisplaySequenceSprite and its owned position; remove the private duplicate cannon layout | `62c8a2f7f`; constructor establishes sprite +0x6C, position +0x88 and 28-byte resource +0x98. Actual task allocation is 0x1A8. All six dependents exact; twelve ARM layout assertions pass. |
| Axe trail / attack overlays | Use the existing native sprite's position and flags directly | `d8fe51260`; complete 29,148-byte unit exact; two unions removed without new casts/helpers. |
| Inventory callback groups | Real 0x40-byte entry with 0x28-byte drawable, measured velocity components, and 8-byte lifecycle/callback tail; eight entries after an 8-byte group header | `cdfd3a2ba`, `0ecff2709`; complete 2,728-byte unit exact, thirteen layout assertions, one dependent. Declared group is now the actual 0x208 stride, replacing the false 0x240 overlay. |
| InventoryGrid owner | Use the canonical HUD and its actual digit renderers through the shared API | `49ac040ef` through `1c3a13828`; six functions total 1,184 bytes exact, eighteen layout assertions, 94 dependents. One union removed; byte-strided flag traversal remains. |
| MenuPrompt owner | Replace panel/account/sprite overlays with native HUD, Player and DarkLoan fields | `821f3a25c`; 1,736 bytes exact and combined HUD gate 105/105. One union removed and four compiler pointer diagnostics disappear; price-renderer overlay remains separate. |
| libgcc double fraction | Retain the authentic whole-word/two-halves union and explain it | `deda4728e`; original pinned FSF fp-bit-base.c uses fractype ll and halffractype l[2], with 64/32-bit types. A justified representation, not a removed source concession. |
| libgcc float fraction | Retain the same original union's FLOAT specialization | Same commit/source, with 32/16-bit types. Both library units remain exact, totaling 5,864 bytes. |

The round therefore removes **nine actual union declarations**: result panel,
RegionMenu, two Vulcan declarations, two Axe declarations, Inventory callbacks,
InventoryGrid and MenuPrompt. The two libgcc justifications are separate. Together
these take the scanner's union reports from 131 to 120. This is not a claim that
all 120 remaining reports are steering, or that unreported overlays are acceptable.

No blanket same-vector exemption or blanket union-removal credit is given.
The final census and source/exemption accounting are in
`notes/round15-priority-recovery.md`.

## Related owner improvements, without union-count credit

RecordMenu's supposed pulse byte buffer is its existing palette[12], at +0x40C
inside the palette at +0x3F4. `87ee4edcb` uses that owner directly and removes the
misleading private byte-array member; all 508 bytes still match. The native
`Palette_UpdateHighlightPulse(u16 *, u8 *)` declaration now lives in
`include/palette_pulse.h`, shared by its defining unit and callers. All 303 header
dependents match, and no new compiler diagnostics are introduced.

The Axe follow-up `5cccc3e03` centralizes the auxiliary sprite, 28-byte graphics and
16-byte sequence at +0xA38/+0xA64/+0xA80 in the real Axe owner. Both Axe units match
(29,148 + 2,708 bytes), and ten field offsets plus 0xF24 size are checked. This
removes duplicate private layouts but does not count as more union removals.
The subsequent `bc01ec9df` uses native DisplaySequenceSprite and the existing
Axe_StepSequence operation for emitted trails; the helper body is shared with
the attack trail, not newly invented. All 29,148 bytes remain exact. The broader
DisplaySequence_StepSprite substitution still misses and is not claimed solved.

The Vulcan effect-position operation copies the actual sprite position and raises
it by 128 for both defeat particles and explosions. Whole assignment without that
operation changed when stack addresses were computed; recovering the repeated
complete operation matches naturally. Native sequence playback also matches all
six users and removes their six pre-existing pointer diagnostics. The existing
facing and explosion-frame literal locals remain warnings.

Inventory's lifecycle uses real entry, renderer and 8-byte state components.
The previous separate cursors are retained with explicit containing-entry stride;
there is no fake array or inflated metadata object. Teardown's byte traversal and
address/dereference counter form remain visible after ordinary alternatives missed.
Its 17 existing incompatible-pointer diagnostics in untouched stat comparison are
identical before/after; the owner recovery adds none. Successful `tools/check.py`
output alone does not establish that compiler diagnostics are absent.

The shared tile-sprite API is owned by the actual 0x28-byte SpriteDrawable prefix,
not by the larger LevelAdvanceParticle. Five defining routines, fourteen
LevelAdvance dependents and the brief player effect match with native fields and
without new compiler diagnostics. SpiralBurst's proven priority/depth/attribute
fields and native API are accepted separately from its still-nonmatching full
owner/projection rewrite. The brief effect's broader private Player-tail model and
the legacy PlayerEffectSprite duplicate are not claimed recovered.

### HUD storage and callback boundaries

Initialization (`08176F70`), teardown (`08176F20`) and the hide loop establish
124 consecutive 0x60-byte widgets from HUD+0x1420 through +0x42A0. The current
shared owner partitions this storage into named groups and partially unknown
ranges. It does not yet provide the common backing-array model required by
generic cross-group indexing; existing uses such as categoryWidgets[62+i] remain.
The new InventoryGrid accesses stay within the first 30 widgets: kind 0 builds
the 4-by-4 selection domain 0..15, and confirmation indexes widgets 10..25.
That evidence does not make the first group an exclusive eight-item array.

MenuPrompt's price renderer still spans the named groups. Independent digit
arrays fold the common flags base into separate bases and produce 392 rather
than 396 bytes; no artificial offset or alias was accepted to hide that miss.
Its raw entry-function views and erased callbacks also remain. InventoryGrid's
older scene declaration still erases the BeginNumberPhase argument: the setter
accepts an integer callback address, stores void* at +0xA54, and dispatches via
Callback_InvokeActor. The actual direct declaration is typed, but no speculative
setter-wide contract was imposed on that independent boundary.

## Unresolved work and handoff

The following remain open and must not be reclassified by a matching-only argument:

- The 32 sequential-role and 26 RNG-address wrappers require ordinary source
  recovery. The one-member RandomTableView comments describing prevented hoisting
  are admissions of steering, not justification for keeping the union.
- Same-vector unions require independent representation evidence or replacement
  with the actual vector operation. Existing packed stores alone do not prove
  that the original used a union rather than fields, a copy or a shared macro.
- Scalar narrowing may be ordinary arithmetic, but pointer-byte mutation and
  one-member masks are separate cases. Do not excuse the whole category together.
- CharacterPreviewModel overlays native display records with metadata at part 2
  +0x12. Identify the actual widget field and producer before changing its owner.
- The actor damage-word/halfword records, menu parameter callback table, other
  inventory/HUD partial views and remaining effect-vector unions are classified
  leads, not completed source recoveries.
- Reported priority holdouts that contain unions remain priority work. The partial
  Vorn, Skeleton, impact and other frontiers explicitly list inherited union or
  helper debt. A warning-free draft is not accepted unless stock bytes match.

Continue by actual subsystem and ROM order within each independent owner thread.
Use the baseline inventory's prior-trial references before retrying a direct split.
For every accepted owner/header change, check its complete dependent set, raw
compiler diagnostics, concession deltas and the integrated ROM. Keep trials for
open sites in the research archive, with one maintained note per topic.
