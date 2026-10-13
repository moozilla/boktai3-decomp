# Text windows, dialogue transitions and wireless controller

Exact coverage: 20.230% -> 20.406%, +0.176 percentage points.
31 more functions and 4,260 reported code-span bytes; 5,626 / 11,109 functions.
Without pinning 16.083%; Register-pinned 4.324%, included above.
Combined with the preceding dialogue batch: 67 new functions, +0.405 points.
One combined build printed `build/boktai3.gba: OK`; the reporting-boundary check passed.
No new matching tools or broad regression work in this batch.

The gUnk_020000E4 window manager links individually allocated windows.
Their text interpreter begins at +0x60 and links are at +0x1C4/+0x1C8.
The reveal callback iterates according to the per-window speed; management
updates can start a new script entry, invoke a callback and run selected
procedures. Destruction unlinks both neighbors before freeing a window.
The remaining creation routine at 08033690 is retained outside production.

All named functions in 08033C58-08034084 now match. This scripted dialogue
transition task reads parallel arrays of delays, fade-in/fade-out times and
rectangles. It drives the already matched dialogue controls, advances script
entries, handles a skip request and restores the saved display register.
Roles are descriptive and provisional; they do not rename linker symbols.

The following block is a wireless connection controller, supported by the
existing RFU SDK evidence in docs/RFU.md. gUnk_03006A80 is RfuLinkStatus and
gUnk_03006A70 is the array of NI slot pointers. The peer packet includes local
and partner names/IDs, tracks connected-slot changes, sends using the RFU NI
API and checks its slot send state. Exact sources use the SDK's existing
structures. The game call site for rfu_clearSlot retains a local wide-argument
prototype, as required by its emitted calling sequence; the SDK callee narrows
its arguments on entry. The event dispatcher updates the controller operation,
connection masks and pending flag, including stop/error events.

All named functions from the wireless lookup helpers at 08034084 through
080345D8 now match. Its following state handlers remain the next contiguous
work area. Existing unnamed assembly tails are preserved; reported spans
include them as before. Register hints and the dispatch tree are temporary
matching concessions; there are no handwritten instruction bodies.
