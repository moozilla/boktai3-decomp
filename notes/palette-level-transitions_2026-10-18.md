# Palette level transitions unit (2026-10-18)

Tags: #palette #mosaic #unit-consolidation

Consolidated 080164B8–0801689C into `src/palette_level_transitions.c` in ROM
order. `include/palette_level_transitions.h` defines the shared 0x34-byte
`PaletteLevels` task: task header at +0x00, state and target selectors at +0x18,
active-channel mask at +0x1B, four timers at +0x1C, periods at +0x24, signed
current sizes at +0x2C and signed targets at +0x30. All nine prior C functions
now use that layout rather than partial structs or byte-offset access.

Despite the existing PaletteLevels names, these functions pack four four-bit
sizes for `Mosaic_SetParameters` (082190E0), rather than altering palette colors.
BG/OBJ horizontal/vertical channel roles and target-selector semantics remain
tentative; retained the existing public names.

Preserved Initialize's consumed return without adding a return statement, and
its unused halfword argument. Create still forwards the first argument and
Configure still passes the unused second argument. Update also retains its
original non-void fallthrough. Cast only the callback pointers at the shared
engine Task_SetCallbacks boundary, whose interface returns void; no engine
prototype changes, partial struct adaptations, pins or barriers were needed.

The already inventoried gap 08016890 is an independent 12-byte clear of
`gUnk_02000078`, the following floating-effect manager's global, rather than the
palette-level task pointer. Added exact C with the file-local alias
`FloatingEffectManager_ClearActive`. No new gap inventory boundary was needed.
No other unmatched functions or NONMATCHING drafts remain in this range.

Validation: nine individual exact checks after type unification; one merged
exact check matched all ten functions and all 996 bytes of the range. No
failed check iterations. Range coverage rises to 100% (+1.205 percentage points,
12 newly matched bytes). Full-ROM validation result is recorded in the history
entry. No commits or git metadata writes; merge_unit.py's --write filesystem
transformation used an adapter replacing its git-rm operation with unlink.

Build setup surprise: the shared generated data assembly referenced resource
archives that were absent from this checkout's build/res. Extracted the missing
ROM-derived manifest sources locally through datafmt.Entry.extract(), without
regenerating or modifying shared gen/; then reran tools/build.py using cached
compilation outputs.
The shared assembly also used suffixed archive filenames from an earlier
extraction. Restored those local filenames directly from the ROM, checking each
filename's archive hash and entry ordinal against its manifest archive. Shared
generated assembly remains untouched.
