# Modifier menu preview and lifecycle (tentative)

Contiguous subsection: `08041908–08042198`. Seven new exact functions add
**0.059 percentage points** (1,428 bytes), bringing the branch to **22.876%**,
6,316 / 11,465 functions and 552,534 / 2,415,354 bytes. The 492-byte validator
is a guarded **71.0%** draft, with no exact credit.

## Observed lifecycle

The factory at 08042158 allocates **0x1088 bytes** on task list 4, installs
08041F3C/08042044 update/destruction callbacks, and runs initialization. Failure
marks the task for deletion and returns null. The shared menu view now extends
to that observed allocation size; its preview storage remains opaque.

Initialization clears byte +0x116, validates saved IDs, loads the pending
snapshot and commits/reconciles it, reads script options, loads text sources,
initializes background/resources/windows, marks every display dirty, constructs
the 3D preview and initializes the per-mode saved choices. The script `E`
option defaults to the observed 0x800 engine flag; `e` supplies the close-script
pointer; `f` enables the 32-frame input delay and opening transition.

The thirteen text banks are script keywords **G F B T S O C f b t s o c**, in
that order. The 08041E04 helper now writes the shared 13-entry `textSources`
array rather than its previous one-entry provisional view. This is a type and
naming correction, not additional matching credit. Gameplay expansions of the
keyword letters remain unconfirmed.

The preview uses resource storage at +0x83C and model storage at +0xE34, key
0x3D99 and origin **(48, 80, 0)**. In engine-flag mode it writes the actor's
+0x18 ID, or 0xFF for a null actor, to menu +0x1086; otherwise it writes zero.
The existing refresh helper connects the model to the saved secondary ID +0x81A.
Calls setting model arguments 16 and 1 remain provisionally interpreted.

The task cancels immediately when the observed actor exists but byte +0x1C is
zero. State 3 updates the closing appearance until `stateFrame > 33`, then
closes the script, clears display/background resources and kills the task.
This proves the earlier provisional `ModifierMenu_OpenSelection` name was
incorrect: 0803FF80 now becomes **ModifierMenu_BeginClose**. Its machine code
is unchanged.

Other states dispatch through gUnk_086058EC. Dirty bit 1 clears/rebuilds label
windows; bit 2 clears/redraws background; bit 4 updates headers; bit 8 dispatches
selection display; bit 16 updates description; bit 32 refreshes the model.
The updater deliberately re-reads flags after callbacks, including the second
bit-2 test. It then updates capacity flashing and appearance, runs the shared
model update and clears dirty flags before advancing the state-frame counter.

Capacity flashing occurs only in state 1, mode 1. During the error countdown,
the low four bits alternate between capacity bars and a cleared rectangle;
reaching zero redraws the bars. The renderer receives `(count, capacity,
capacity, 8, 16)`: the repeated capacity argument is observed, and substituting
16 for the third argument was an incorrect first hypothesis.

Appearance cycles a color at menu +0x130 (palette entry 12) using byte +0x117.
State 1 writes 0x73FF to global palette entries 17–23 and 25. Other states call
the +0x3C controller update and model +0xE34 update with argument zero. Graphics
storage and gameplay identities beyond these accesses remain tentative.

## Saved selection validation draft

08041A44 clamps source IDs for kinds 0/1/2 and the secondary ID for kind 5,
checks availability and chooses fallback IDs. Kind 3 becomes -1 when unavailable.
It loads capacity from the second-source record's byte 3, validates four primary
IDs as kind 4, collects them in a mask, clears their slots, then writes selected
bits back in ascending ID order up to capacity. Secondary fallback uses six
entries per category from gUnk_086058E8[2].

The retained C has the reviewed 492-byte extent and 71.0% fuzzy score. Diagnostics
show global-pointer reuse across adjacent assignments, store-address registers,
mask-loop rotation, and allocation differences. A setter-helper experiment
produced 512 bytes; a memory-barrier/fixed-counter experiment reached 74.7% but
516 bytes and extra saves. Both were rejected in favor of the readable full-size
candidate. Scratch evidence: build/modifier-validate-saved-base.c and check/regdiag
logs. No blind repeat of those variants is warranted.

## Matching evidence and next step

Reviewed extents: 0804194C 212; 08041C30 224; 08041D10 152; 08041DA8 92;
08041E1C 244; 08041F3C 264; 08042068 240. Each exact addition has a separate
one-unit commit. Hint minimization stripped all hints from flashing, appearance,
and initialization. Preview initialization retains one empty barrier; the saved
loader retains two register hints; task update retains three hints/barriers.

The loader required integer addition with the row displacement first to retain
operand order. The first r0-pinned ID variant overwrote the call argument before
copying it; the accepted variant was corrected and checked byte-for-byte. Explicit
unsigned negation extracts the nonzero flag of a value known to be zero or 0x800,
allowing initialization to match without pins. Empty barriers in the task preserve
boolean materialization and prevent factoring its two cancellation call sites.

Shared actor/menu views, named color fields and text-source writes were rechecked
at full extent after type cleanup. The complete combined build printed **`build/boktai3.gba: OK`**
(`build/modifier-preview-combined.log`); format, inventory and RAM-literal
validation passed. The next contiguous subsection starts at
**08042198**: a script/text cursor and its adjacent scrolling task, separate from
the completed modifier menu lifecycle. The validator remains available for a new
allocator hypothesis.
