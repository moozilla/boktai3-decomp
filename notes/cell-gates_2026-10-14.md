# Cell-based traversal gates and the preceding script entry

Three new gate functions add 856 exact bytes. The complete
**08050128–08050820** subsection is now exact: 1,784 contiguous bytes.
Eleven existing functions have provisional names; ten use the recovered
view directly. The initializer retains one minimized callback register hint;
the two new behavior callbacks need none. No new runtime coverage is claimed.

`include/cell_gate.h` describes the observed 0x3C-byte task. Fields:
ID18, flags1C, terrain-registration flag1D, cell1E, height20, player-inside
counter flag22, sixteen-byte terrain registration24, script resource ID34,
and selected callback38. The player view distinguishes position30/32/34,
action mode418, transition mode457, direction4A5 and gate count4E3.
The gameplay name `CellGate` is provisional; these are observed traversal
constraints, without a claim about a particular visual object.

## Three behaviors

The initializer506CC reads `t` (default9), packed XYZ `p` (default0), and
optional script resource ID `R`. It converts X/Z to map cell coordinates by
arithmetic shift8 and uses the map's row table, selecting cell0 outside the
map. It stores Y shifted8 as height. Flag1 selects50280; otherwise flag2
selects50404; otherwise505BC. Allocation is task list8, size0x3C.

The tick50694 skips the selected callback when player word1C has mask 4.
The destructor506B8 unlinks the terrain registration only if registered.

- **50280 passage barrier:** require equal player height. A player in the
  cell who passes501E8 removes the barrier, calls the player passage API and
  optionally calls the script. Otherwise install terrain entry82A. Leaving
  the height removes the registration. The script ID is retained after use.
- **50404 forced direction:** on entering the cell at matching height,
  increment player4E3, set direction7 for flag8 or direction1 otherwise,
  remove the terrain registration and clamp the axis at local coordinate40.
  When transition/action modes permit, call the player direction API.
  Leaving the cell decrements the count and may install terrain entry80B,
  depending on50310. Leaving the height decrements and removes as needed.
- **505BC conditional barrier:** traversal mode50574 clamps to64 while in
  the cell. Otherwise a live registration is removed and the passage API is
  called, except flag20 retains it and sets registration halfword+6 bit40.
  Either path can call the script. Outside the cell, install entry82A only
  when traversal mode and the axis-edge test pass; otherwise remove it.

Flag8 selects X as the constrained axis; otherwise use Z. The edge test masks
the coordinate to its low byte and compares it with the limit. The clamp
preserves the signed cell component and copies the resulting coordinate to
player cache168/16C. Height comparison uses the unsigned stored height shifted8
against signed player Y. Preserve those widths and the global pointer reloads.

Passage eligibility distinguishes transition4/direction1 or7 from action3/4
and global0534/0500 conditions. The two eligibility helpers differ in when
an axis-edge check is required. Existing byte views were retained where their
load/alias behavior matters. Names describe the current static interpretation.

## Preceding manager script entry

**0805002C (252 bytes, no hints)** reads `n`, packed XYZ `p`, then `s/h/f/m`,
all defaulting to0. It creates manager02000110 if absent and, while count18 is
at most7, initializes one entry through4FEFC and increments the count.
Each entry occupies0xFC bytes beginning at manager+1C. The provisional name
`RepeatedEffect_AddFromScript` does not assert a particular gameplay identity.
The adjacent worker owns recovery of that manager's lower-address functions.

Together with the staged effect batch, exact code reaches **24.509%**, up
**0.1209 percentage points** (+2,920 bytes). The combined build printed
`build/boktai3.gba: OK`; 33 complete post-promotion extent checks passed,
as did format/inventory/RAM checks and all twelve data round trips.
