# Rectangular area effect manager (Sol, 2026-10-14)

Adds **0.0795 percentage points** exact code (1,920 bytes, ten functions).
Of these, 252 bytes are unpinned and 1,668 retain minimized register hints or
empty barriers. This continues the staged effect batch's 0.0750-point gain,
for **0.1545 points / 3,732 newly exact bytes** across both subsections.
All full reviewed extents pass fast exact checks; combined ROM validation is
reserved for the integrator. No emulator evidence is claimed.

The contiguous **0804E008–0804EBD8** range is now exact (3,024 bytes,
including existing functions). `AreaEffect` is a tentative subsystem name:
script-defined rectangles optionally display sprites and influence a player
resource accumulator. Exact gameplay identity is not yet established.

## Shared layout and contracts

`include/area_effect.h` describes the manager behind **0200010C**. Its creator
allocates 0xAB4 bytes, precisely the 0x2C-byte prefix, sixteen 0xA8-byte entries,
and four trailing fractional counters. Prefix: player18, initial particle mode
1C, resource fraction1E, idle frame20, action frame22, graphics ID24, count26,
mode27, sprite visibility28, one-frame override29.

Each entry has center XYZ at0, sprite flags8, sprite render modeF, priority18,
display XYZ24, two 0x1C-byte graphics objects60/7C, tag98, enabled9A, kind9C,
hasSprite9D, cached terrain value9E, rectangular boundsA0/A2/A4/A6. Indexing is
consistently 0xA8 bytes and the script adders cap count at16. `E030` scans enabled
entries and returns the first inclusive X/Z rectangle containing the supplied
signed position, or -1. Its bounds are unsigned halfwords, as in the assembly.

`E180` uses two four-entry halfword radius tables at08605A78/08605A80; rotation
swaps the selected X/Z radii. `E214` loads display resource0xCBE1, selects the
manager's graphics ID, sets priority from kind and render mode3, derives bounds,
and caches the terrain query. `E758` selects graphics1C8 for environments3/4,
1C9 for5, and1C7 otherwise, refreshing both graphics objects for visible entries.

Script contracts: keyword6E selects an entry tag;61 gives a direct four-bound
rectangle;70 gives sprite center XYZ, whose Y is replaced by observed terrain
height;74 selects kind (default0);72 selects rotation (default0). Enable/disable
helpers affect the first matching nonzero tag and update sprite flag value1.

## Resource and particle behavior

`E618` clears idle time on engine flag100 or nonzero03005260, otherwise advances
it through900. It skips accumulation for masked engine flags200 and02000560 bit4.
Player action mode6 gains40 fraction units in mode2, or5 in mode0 while idle<=899.
Other action modes use signed GameState+876 times8 in mode2, or its base value in mode1
while idle<=899. Action mode2 multiplies the accumulated fraction by4. Every120 units
are converted to one increment through081594F4, which clamps player+428 against
player+42A. `E730` selects mode2 from player flags20, otherwise mode1/0 from10.
The exact gameplay name of player resource428 remains tentative.

`E8D8` shows enabled sprites and may emit action particles when player state tag1
and a matching area (or GameState flags868 bit2) allows it. Player action457 must
be7 and phase458 must be2/3. During action frames0..62 it emits every eighth
frame from player position offset(-100,+256,-100), with extent(256,0,256), then
advances the action frame. It invokes the graphics refresh and resource updater.
Packed word masks preserve the original padding and access widths.

## Newly exact reviewed extents

E18096, E214152, E2AC220, E388428, E618280, E73040, E758168, E800124,
E8D8352, EAFC60. All ten are individual matching translation-unit commits.
Thirteen pre-existing exact helpers were also named and checked in full.
Known canonical engine APIs (Sprite_Init, AnimData_Load, AnimData_SelectTiles)
are used in new code. No new drafts or gaps remain before EBD8.

Continue with EBD8 onward, the adjacent multiplayer form of the area updater.

Integration: combined orbit/area ROM build printed `build/boktai3.gba: OK`;
51 complete post-promotion extent checks passed, along with format/inventory/
RAM checks and all twelve data round trips. Aggregate exact progress24.682%
(+0.1726 percentage points for this batch).

Review correction: player+418 uses the existing player view's `actionMode` name;
word1C is tentatively `stateTag`, observed active value1. These are distinct
from the action/transition bytes457/458. No additional exact credit.
