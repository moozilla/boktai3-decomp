# Modifier selection and stat previews (tentative)

Tags: #modifiers #menu #matching #unit-consolidation

Range: 0803F804–080408A4, end exclusive. Consolidated the 44 input files into
`src/modifier_selection_and_stat_previews.c` in ROM order. Reused
`include/modifier_list.h`: these are the same ModifierSelection/ModifierMenu
objects used by the surrounding menu code. No new separate header is needed.

## Types and interpretation

The 0x28-byte ModifierSelection holds two signed stat groups, count/capacity,
selection mask, four source IDs and primary/secondary IDs. Converted the raw
snapshot/menu accesses in loaders, source commits, packed-value reads and
background setup to these shared types and GameState fields.

Added ModifierByteSourceInfo for the eight-byte second-source lookup result:
byte id, kind-zero and kind-seven values, byte capacity, halfword kind-six value,
and two unknown bytes. Numeric stat-kind interpretations remain provisional.
The other lookup buffers remain word arrays matching their existing APIs.

ModifierMenu +0x60 is now `backgroundScript`, a resource passed to
Bg_SetupLayers; it is distinct from the close-script word at +0xB8. The
former unknown3C region was split without changing any offsets or total size.
The packed six-bit values use the existing GameState word fields, including
signed arithmetic shifts through a signed local.

The existing Modifier_EnumerateSelection file-local alias is retained (its
previous duplicate define is removed). All other understood functions already
have promoted descriptive names; no symbol-table edits were needed. No gaps
were added.

## Matching and adaptations

43 existing C functions remain exact; 4,000 C bytes out of the 4,256-byte
range, 93.985% (+0.000 percentage points). No new exact functions.
The 256-byte Modifier_CollectSources stays as guarded NONMATCHING C plus
INCLUDE_ASM. Ten bounded collector checks improved its recorded fuzzy score
from 88.1% to 94.5%. Loading the GameState pointer before the selected ID and
forming the ID pointer afterwards improves the scan's register allocation.
Remaining differences include spill/load order, registers and a hoisted sort
pointer; the candidate emits 260 bytes. It earns no exact credit.

The existing GameState modifierIdsBytes signed-halfword adaptation remains:
prior evidence showed an s16 array changed store lowering. No additional raw
menu-object views or per-function partial structs remain. Existing register
pins and the empty mask barrier remain only in their original functions;
depin.py tested all 12 hints and removed none. No hints were added.
Engine declarations come through modifier_list.h -> engine.h. The existing
Script_Call key argument is passed from the opaque menu script word, as before;
there was no need to change an engine prototype.

Consolidation used tools/merge_unit.py's merge() implementation, with ordinary
filesystem deletion of inputs because its CLI --write uses git rm and this
worker is forbidden from writing git metadata. No tools were changed.

## Validation

- Full merged-range exact check: zero differing bytes over 4,256 bytes, including
  the collector's assembly fallback.
- NONMATCHING --record: collector 94.5% fuzzy, scored separately from the unit.
- make format completed.
- Progress before/after: exact function and byte counts unchanged; register-pinned
  classification unchanged.
- Full build: tools/build.py printed `build/boktai3.gba: OK` (log:
  build/unit_modifier/build-retry.txt). The first run completed C compilation
  but lacked the checkout-local build/res inputs. Copied those generated
  resources into this checkout and reran successfully; shared gen stayed read-only.

Rough checking effort: about ten collector checks, several merged-unit checks,
and the depin pass over twelve existing hints. No shared docs or progress totals
were edited, and no git commit was run.
