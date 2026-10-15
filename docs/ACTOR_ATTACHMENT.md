# Actor attachment offsets

Reviewed geometry subsection 080648B0–08064D40. Nine offset helpers/dispatcher are exact for their complete reviewed extents. Two newly recovered functions add **0.0166 percentage points** (400 ordinary-C bytes); the previously exact helpers gain types and provisional names without additional credit. The already recovered hidden 64B20 and independent 64D3C leaf boundaries remain in existing extra-function metadata. The 64C48 dispatcher ends at 64D3C, not at the next ordinary disassembler label 64D40.

`actor_attachment.h` describes the pure offset API: kind and facing quadrant select signed output coordinates. 64C48 computes quadrant as `((angle+0x20)&0xFF)>>6`, then dispatches action4 to 648B0, action1 to 64918, action3 to 64954, actions5..8 to 64990, action21 to 64A58, action25 to 64B70, and other actions to 64BF8. The fourth argument is passed to all helpers even when unused. Invalid kind/direction paths preserve caller output values. The deeper kind/action identities remain tentative; no runtime actor identity or screen-space coverage is claimed.

64990 and 64A58 retain their nested switches and all source constants. Their kind0 and kind3 tables depend on four facing quadrants, kind1 distinguishes directions0/3 from others, and kind2 shares the common offset rule. No register hints or handwritten instruction bodies were needed.

Full reviewed exact extents: 648B0=104,64918=60,64954=60,64990=200,64A58=200,64B20=80,64B70=136,64BF8=80,64C48=244. The already exact 64D3C leaf is separate and has four bytes. Complete combined-ROM validation belongs to parent integration.

## Slot state and jitter

The completed contiguous prefix is now 08064790–08064DF8: thirteen full reviewed function extents, including the existing four-byte 64D3C leaf. Three new functions add **0.0242 percentage points** (584 bytes:400 ordinary and184 register-concession bytes); existing typed/unchanged code contributes no new credit.

`actor_attachment_state.h` holds an extended partial view distinct from the neighboring worker's narrow actor_attachment_slot.h. Static accesses establish three jitter vectors at4, twelve bytes of flags/state at30, status[3]36, phase[3]39, partMask3C, signed slotId3D, timer40, thresholds[3]42, a signed value pointer4C and counters[3]50. The main sprite starts60, three 0x28-byte part sprites start8C, and their 0x10-byte sequence states start104. A follow-actor pointer occupies134, yielding the observed0x138 slot stride. Manager slots start5C and the eight-bit occupied mask is atA1C. Slot kind3E and resourceKind3F remain provisional names. No broader actor identity is asserted.

64790/64820 apply cumulative horizontal shake while timer40 is positive and status[0] bit1 is active. They advance the global random-table index once per applied shake and add offsets in[-10,9] or[-1,0], respectively. Timer decrements on every positive-timer call, even if shaking is disabled. At zero or below they clear bit1 and stop changing the position.

64D40 activates a part's jitter when the global manager exists, the pointed signed value is at or below the part threshold, and that part status byte is not exactly1. Three successive RNG entries seed X/Z in[-8,7] and Y in[0,15]; only the third index is written back globally. The part's status gains bit1 and its counter resets to zero. The output records are typed packed-coordinate views; each coordinate is written separately and padding is not read.

The initializer retains six minimized register hints after depin19→6. Explicit mutable sample loads/masks avoid an old-GCC scratch copy and keep the owner naturally in r7; r7 is never explicitly bound. The index adjustment precedes the structure member offset, preserving the target halfword stores at+4,+6,+8. Shake functions remain ordinary C and full exact checks pass.
