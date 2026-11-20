# Link, hardware, and minigames warning cleanup

## Coordinator wind-down

The STOP marker arrived at 01:15:56 PST. The branch clears 107 of the 226 original queue warnings, including the NI payload exemption. Exact coverage is 100% on the assigned baseline, a change of 0 percentage points. Per-unit and header-dependent stock checks covered the landed code.

- Wireless scene objects, 0803BAA0, was interrupted. The recovered actor and spatial entry layout passes 69 dependent checks. 0803BC88 uses those types and matches all 896 bytes. Its commit message overstates the timer cleanup. The zero local remains, as its direct form loads the zero after the duration read. The unit remains at 17 queue warnings and 20 with late literal detection.
- The counted free-slot scan and shared slot-release and proximity-cooldown drafts match their target functions at 0803BAA0, 0803C008 and 0803C070. The scratch drafts in build/cx/work/scene-pool were left out at wind-down. The scene-object remainder and queue units 11–29 remain open.

## Finished

- Wireless result panel, 0803AC9C. 4 → 1 on the queue, 6 → 3 with late literals, and 8 → 5 with the new dead-assignment detection. All 18 files and 3,588 bytes match. Typed display handlers, a forward widget loop, direct vertex and state clears and the result show name landed.
  - 0803B40C resource guards remain open. Their removal loses the two inverse pointer copies at 0803B458 and 0803B486. Built-in and library copies, positive branches and the shared sequence operation did not retain them. The new detector reports both assignments.
  - 0803AC9C and 0803B1E4 still need the flag and point-cap constants before loop setup and descriptor selection. Direct values and the existing clamp differ by one instruction. 0803BA90 needs the mask load before the high-key shifts. Direct operands and computed fragments still place it later.

- Wireless game items and display, 080395D4. 24 → 9 on the queue, 26 → 10 with late literal detection. All 5,832 bytes and both header dependents match. Native resource/vector recovery, shared phase and frame operations, typed index loops and five symbol names landed.
  - 0803A6F8. Indexing both the pending count and the item prevents the premature item-pointer induction. The address trick is gone. The single iteration state block and two distinct clears remain open. The clean indexed test leaves the pending-byte store using the other zero.
  - 0803A844 remains open. Removing the random table union changes the resource/table homes and seed reloads. Direct, shared sampler, fixed address and common shake operation all differed. Best 19 ROM / 13 draft rows with direct fixed-address sampling.
  - Other open sites: 0803962C late counter saturation mask, 08039DE4 outgoing reset pair, 0803A068 drift scale, 0803A514 queued timer zero, 0803A744 hidden flag. The direct forms respectively materialize the mask early, lose the split clear, change the fixed-point tail, move the branch clear, and swap the flag constant with the loop counter.

- Wireless trade and selection menu, 080380C4. 37 → 10 on the original queue, 38 → 10 with late literal detection. All 26 files and 5,136 bytes match. The shared menu and peer layout, callback table, session points and elapsed times replace the private views. The native task size is 276 bytes.
  - 08038448 remains open. The typed numeric-table draft loses the unused SL save and changes the seed-pointer reload. The symbolic table restores both homes but hoists the table address. Direct indexing, cursor and range alternatives, and the shared random-draw macro preserve that split. Drafts in build/cx/work/equipment.
  - 08038838 and 08038AD4 retain the packing initializer one-pass loops. Flattening changes low-register reload choices and the count loop. Shared nibble-pack inline and macro variants also differed in both callers. The active round's direct defeat fields, loop accumulator clears and shared phase transitions match.
  - 080389AC uses the actual save slots, and the default equipment-mode stores are direct. The three phase locals remain open. A direct timer clear moves its zero past the state field address, and a direct mode-one write changes how both switch tails share their value.
  - 08038CDC remains open for the player slot mask. Both direct operand orders move the mask relative to the status word load.

- Wireless packet exchange, 08037B20. 1 → 0 on the queue, 2 → 1 with late literal detection. All eight files match. The NI payload exemption documents the same word the sender copies and the receiver also reads in halfwords. In 08038074 a direct count-clear literal moves the zero after the last-byte address. Descending pointer, array-index and address forms did not restore the order. The late-literal warning remains open.

- Wireless peer panel, 08037228. 8 → 0 on the queue, 9 → 0 with late literal detection. SDK destination evaluation gives the original scratch clear. The indexed widget accessor reaches the actual digit fields, and the menu shares its typed state transition. Every function in the 2,296-byte unit matches.

- Wireless status menu, 08036E08. 3 → 0 on the queue, 4 → 0 with late literal detection. The common pause setter and direct transition and widget arguments match all 1,056 bytes.

- Wireless connection controller, 08034084. 25 → 0 on the original queue, 30 → 1 with late literal detection. Zero errors.
  - The shared task, roster, and RFU packet layouts replace the private views in the edited handlers. Direct member stores admit the reset, operation transitions, NI flags, and input publication. All 19 dependents of the final controller header match.
  - 08035320 and 08035958 share the parent acknowledgement macro. Both expansions match. Direct halfword flag reads and forward slot scans clear the NI one-pass loops in 08035F5C and 080362E0.
  - Open 08036910. The constructor now uses the shared task size and direct random cursor mask. The byte packing mask still needs to be materialized before the palette-phase byte load. The direct mask, narrowed byte and structured pack variants differ. Search in build/cx/work/search-36910.
  - Existing 0803559C sequence +0 remains source debt outside the linter. Its removal exchanges two PRE address spills. The diagnostic table size 53 matches, and the natural alternatives so far either keep size 51 or change other allocations. Best draft build/cx/work/3559C-clean.c.

- Result game tutorial, 0801D104. 8 → 0, zero errors. Named widget access admits the direct prompt flag. The phase and step increments admit direct resets, and the normal indirect call generates the original callback veneer. All 3,836 bytes match.

- Transformation and result game, 0801B49C. 17 → 0 on the original queue, 21 → 0 with late literal detection. Zero errors.
  - 0801BAA8. The burst index times the angular spacing lets the particle arguments be direct literals. Shared ResultMenu and Player types preserve the reveal vectors.
  - 0801BCA8 and 0801BD44. Forward indexing and the shared sprite types admit the SDK position clear and direct widget and origin parameters. The overwritten marker x store was absent in the ROM.
  - 0801C0C8, 0801C5E4, 0801C69C, 0801C874, 0801CAF0 and 0801CC24. Typed stores and forward feedback traversal admit direct clears. Target tracking keeps the two interleaved difficulty table columns as distinct base addresses.
  - 0801B5B4. The acknowledgement work revealed that a typed byte-field store can make the compiler reuse an early zero. The shared equipment fields and the definition’s named grade column preserve both the clear and the lookup order. All 140 equipment-header dependents match.

## Annotations and tools

- `WirelessGameDrawing.position` `local-union`. Motion edits the three coordinate halfwords and sprite placement copies both words of the same eight-byte position. The native coordinate-struct assignment trial changed the item routines' sizes. Both header dependents match.
- 08037DEC `local-union`. The sender copies one NI payload word. The receiver reads that word for counts and decodes its two halfwords for the timer and flags. The word-only candidate rotated the state and mask homes. The halfword-only candidate changed the ROM's word loads.
- The literal-local linter now reports standalone late literal assignments. It found 488 additional sites across src/ and include/ at the comparison revision. Four such sites are resolved in the first unit. The concessions suite passed 38 tests.
- Added a narrow source proof for dead assignments guarded by a signed negative test immediately after unsigned narrowing. It reports two previously silent sites in 0803B40C and two in 081D3620. Compiler reports at the same line are deduplicated. All 40 concessions tests pass.
- Scope why all reports instruction motion across diff hunks. Verified on the result marker initializer experiment and passed 25 related tests.
- The shared result-menu header passed checks of all its dependents after every change.

## PARAMETER HELPERS

- `WirelessStatusMenu_AddPauseFlags`. 2 callers, 2 sites. Needed for the match. It wraps generic pause-bit arithmetic and both call sites pass 4.
- `WirelessPeerPanel_FillName`. 1 caller, 1 site. Needed for the match. The typed byte-range walk is real and its sole caller passes zero.
- `WirelessPeerMenu_SetState`. 1 caller, 2 sites. Needed for the match. Both branches reset entry state and timer, and one passes a literal.
- `WirelessPeerPanel_GetDigit`. 2 callers, 3 sites. Real operation. It locates a digit widget by runtime index using the recovered widget layout.
- `WirelessController_SetOperation`. 8 callers, 16 sites. Real operation. It changes controller operation and resets the stage, frame counter and entry latch as one transition.
- `WIRELESS_RECORD_PARENT_ACK`. 2 callers, 2 sites. Real operation. The manager and completion callbacks report the same acknowledged slot set and run the same completion test.
- `WirelessGameMenu_EnterState`. 13 callers, 16 sites. Real operation. The same typed operation sets the phase and entry latch and restarts the timer across the startup, protocol, active round and results handlers.
- `WirelessGameItem_EnterState`. 5 callers, 5 sites. Real operation. Blinking, merge, arrival and queue handlers all put an item into its steady phase and reset the same entry latch and timer.
- Existing `Item_LoadFrame` now has 2 callers and 3 sites. Real operation. Steady and temporary item animation select and display the same frame through this path.
