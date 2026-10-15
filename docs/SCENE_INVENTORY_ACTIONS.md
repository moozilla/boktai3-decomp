# Scene inventory action display and transition control

Reviewed 08060344–08060708: four new functions are exact (652 bytes, +0.0270 percentage points), with 484 ordinary-C bytes and 168 bytes retaining one minimized allocation hint. Existing 60528 is typed/exact for its full 52-byte extent, with no new credit. The fifth new function, 6055C (260 bytes), remains an ignored scratch draft at 99.2%, not counted as matching. Its loop, stack layout and outgoing call arguments are exact; only the reverse-copy pointer setup versus counter initialization is reordered. Resume source: wip/sub_0806055C.c, checkpoint build/check/run-n9aq_sbq.

60344/603EC/60488 compute equipment, item and consumable values for the saved selection and show four digits at the value display positions. They hide the display when no usable selection is present. 60528 searches eight equipped slots for the supplied primary inventory index and returns -1 when absent.

60660 hides the selected primary widget, constructs one enabled transition from the saved equipment position to grid position 26, supplies its equipment frame plus 0xF0, and disables/clears the second transition. Both attribute words are initialized to 0x373, even though the currently reviewed transition callee does not consume that argument. Duration is 12 ticks and sound is 0x38A. Its six coordinate stores preserve X/Y/Z order without reading padding.

The 6055C draft hides both selected primary widgets and any equipped digit widgets that correspond to them. It builds enabled flags and equipment frames, records both grid positions, then reverses those positions into the destination pair. Empty kind 0xFF disables the corresponding widget. It uses the same attribute words, duration and sound as 60660.

The transition pair at owner+0x1AF0 overlaps the catalog array view beginning at 0x1AB3. These are phase-dependent views, not independent simultaneously stored fields. The dedicated action and controller headers preserve that distinction. No runtime scene coverage is claimed. Parent integration owns canonical names and complete-ROM validation.
