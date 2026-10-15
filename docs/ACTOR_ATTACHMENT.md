# Actor attachment offsets

Reviewed geometry subsection 080648B0–08064D40. Nine offset helpers/dispatcher are exact for their complete reviewed extents. Two newly recovered functions add **0.0166 percentage points** (400 ordinary-C bytes); the previously exact helpers gain types and provisional names without additional credit. The already recovered hidden 64B20 and independent 64D3C leaf boundaries remain in existing extra-function metadata. The 64C48 dispatcher ends at 64D3C, not at the next ordinary disassembler label 64D40.

`actor_attachment.h` describes the pure offset API: kind and facing quadrant select signed output coordinates. 64C48 computes quadrant as `((angle+0x20)&0xFF)>>6`, then dispatches action4 to 648B0, action1 to 64918, action3 to 64954, actions5..8 to 64990, action21 to 64A58, action25 to 64B70, and other actions to 64BF8. The fourth argument is passed to all helpers even when unused. Invalid kind/direction paths preserve caller output values. The deeper kind/action identities remain tentative; no runtime actor identity or screen-space coverage is claimed.

64990 and 64A58 retain their nested switches and all source constants. Their kind0 and kind3 tables depend on four facing quadrants, kind1 distinguishes directions0/3 from others, and kind2 shares the common offset rule. No register hints or handwritten instruction bodies were needed.

Full reviewed exact extents: 648B0=104,64918=60,64954=60,64990=200,64A58=200,64B20=80,64B70=136,64BF8=80,64C48=244. The already exact 64D3C leaf is separate and has four bytes. Complete combined-ROM validation belongs to parent integration.
