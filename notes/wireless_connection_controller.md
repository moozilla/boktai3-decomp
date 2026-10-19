# Wireless connection controller unit

Tags: #wireless #types #matching

Range: `08034084–08036CA4`, end exclusive. Consolidated the 100 input
files into `src/wireless_connection_controller.c`, in ROM order, using
`tools/merge_unit.py`'s merger. Its CLI deletion step uses `git rm`, so inputs
were removed directly after calling the merger; Git metadata belongs to the
driver. No commits, shared documentation edits, or generated-source edits.

## Shared layout

`include/wireless_connection_controller.h` defines the observed 0x38C-byte
`WirelessController`, reuses the engine's `Task` and `GameState`, and reuses
`rfu_compat.h` for RFU link/slot types. It replaces all per-function partial
controller, game-state, and RFU-status structs. Field names are provisional.
The controller groups connection state, peer selection, game identity,
UNI/NI buffers, sequence/acknowledgment counters, aggregate input/history,
configuration, roster, packet queue and four transfer callbacks. Compile-time
checks establish its size and roster/queue/callback offsets.

`WirelessControllerEvent` unifies the byte notifications and the halfword
code/value event at `gUnk_03005390`. Its halfwords must stay flat: agbcc pads a
nested two-byte union to four bytes and would move the value from +0x16 to
+0x18. Likewise the 0x11C-byte-offset configuration remains a byte buffer,
not an invented SDK object type. Packet buffers stay wire bytes. Child UNI storage is 20 bytes: a four-byte
send record followed by a 16-byte receive record. The encoded user-name
storage includes its terminator (nine bytes).

The word at +0x4C is a script key passed to the existing engine `Script_Call`
API, not an object pointer; the separate identity-script pointer is +0x388.
The setter at 080341B0 writes the saved input at +0xEE; the active aggregate
input is +0xEC. These names follow the observed accesses.

## Hidden functions and names

The input-file inventory does not show functions inside retained incbin tails.
Four additional entries were reviewed and registered in the build/progress
boundary CSVs (the generated assembly is unchanged):

| Address | End | Result / provisional role |
|---|---|---|
| 080347D8 | 080349F0 | Exact; `Wireless_OnChildConnectionEvent` |
| 080359E4 | 08035B98 | Guarded draft, 86.7%; parent UNI session initializer |
| 08035B98 | 08035CD0 | Exact; `Wireless_BeginChildUniSession` |
| 08035CD0 | 08035CE8 | Exact; `Wireless_ClearSlotsIfActive` |

The three exact functions have file-local descriptive aliases. Existing aliases
for receive-if-created, local participant index, NI copy, and local identity data
were retained. The parent initializer's residual differences are counter-pointer
allocation, receive-buffer address hoisting, stack slots and a few store/load
ordering choices. No new pins or empty barriers were added. It remains
`#ifdef NONMATCHING` with `INCLUDE_ASM` in the default path, as required by the
unit-worker brief. The best of ten parent checks is recorded with
`tools/check.py --nonmatching --record`.

## Matching adaptations

* The RFU game's preexisting int-width `rfu_clearSlot` prototype remains a
  documented header adaptation; the SDK definition itself takes byte arguments.
* The peer handshake needs base-plus-slot followed by the handshakeWork offset;
  ordinary typed array indexing reverses those additions. This minimal byte-base
  adaptation is commented at the access.
* `Wireless_UpdateParentUniSession` retains a small byte-base block for the
  acknowledged/received/waiting flags: typed members changed register/store
  order and added four bytes. Its typed version was checked before retaining
  the adaptation.
* Existing constant-register address idioms now derive their constants from
  `offsetof(WirelessController, ...)`, preserving the checked allocation.
* Engine `Task` has no named field at +0x16. The constructor documents its byte
  write within `task`; +0x14 uses `task.list` and +0x10 uses `task.id`.
* Two original no-argument `Bg_ClearTilemap` call sites preserve that calling
  shape through a visible function-pointer cast; engine.h's normal prototype
  requires a background argument. The operation-entry reset leaves r0 zero at
  these sites. No engine.h declaration was changed.
* Ran `tools/depin.py --write` on all 100 inputs: 109 existing pins/barriers
  tested, all 109 still necessary in that pass. They retain the concession
  markers and count in the Register-pinned category.

## Validation and progress interpretation

All original 100 C functions pass individually with the shared types. The three
new exact functions pass reviewed-size checks. The final formatted default unit
passes a **full 11,296-byte** exact check, including its retained assembly.
The guarded parent draft is independently recorded at 86.7%.

The old progress inventory credited both hidden initializer tails to the
preceding exact C function. Splitting the entries corrects that accounting:
there are three newly exact functions (872 bytes), while the remaining 436-byte
assembly initializer is no longer credited as C. On a baseline with the same
four reviewed boundaries the source gain is **+0.036 percentage points**. Against
the original uncorrected report, the displayed total decreases **0.018 percentage
points**. This is an accounting correction, not a regression of an existing C
match; the UNIT_WORKER no-decrease check cannot hold simultaneously with honestly
registering this previously credited assembly entry. Totals remain in the normal
progress report, not this note.

Rough effort: about 30 substantive check iterations across layout adaptations
and gaps, plus the 100 per-file checks and the depin validation pass. The child
initializer matched on its eighth check; the child event handler on its second;
the slot helper on its second. The parent draft was retained after ten checks.

The first build reached data assembly but this worker checkout lacked the
ROM-derived `build/res/` inputs used by shared `gen/data.s`. Extracted the local
manifest resources and text bank without touching `gen/`. Some shared assembly
references use indexed filenames from a prior extraction; supplied those local
files only after checking both the archive index and hash against the freshly
extracted index. No build tooling or tracked assets were changed.

Full combined build: **`build/boktai3.gba: OK`**. The final 16 MiB ROM
passes the build tool's original-ROM SHA-1 comparison. Formatting and
`git diff --check` also pass.
