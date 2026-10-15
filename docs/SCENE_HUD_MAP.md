# Scene HUD tilemap helpers

Reviewed range: **0805DD18–0805DE3C**, five functions, 292 bytes. Two new functions add **164 exact bytes (+0.0068 percentage points)**, both register-pinning concessions with two minimized hints each. Three existing functions account for128 bytes and receive typed contracts. All full reviewed extents match; DE3C starts the next HUD consumer.

`scene_hud_map.h` overlays only the map pointer at2C in the shared 30-byte background record. It uses the existing BackgroundLayer declaration, avoiding a conflicting global type. Map coordinates wrap independently to0–31 at lookup; the address uses a32-cell row stride. Subsequent runs advance linearly and do not wrap each element.

| Address | Name | Bytes | Contract |
|---|---|---:|---|
| 0805DD18 | SceneHud_GetMapCell | 32 | Return map[layer] + wrappedX +32*wrappedY. Caller supplies a valid layer index and map. |
| 0805DD38 | SceneHud_FillTile001 | 48 | Fill a positive-length run on layer0 with F001. A nonpositive count writes nothing. |
| 0805DD68 | SceneHud_FillTile020 | 48 | Same run behavior with F020. |
| 0805DD98 | SceneHud_DrawDecimal | 80 | Repeatedly divide by the caller's positive place divisor, emit the quotient as a glyph, retain the remainder, then divide the place by10. Glyph base is `(style<<4)+21`; each glyph narrows to16 bits before OR withF000. Writes no cells for a nonpositive initial divisor. |
| 0805DDE8 | SceneHud_DrawCenteredMeter | 84 | At layer0 cell(5,2), write eight cells from selected/unselected template rows. Select a centered interval of caller-supplied length, using the centering table0824DEB8. Valid table indices correspond to lengths1–8. |

The decimal helper emits leading zeroes and does not clamp its quotient to0–9; its caller is responsible for a suitable value/divisor pair and style. Signed division and remainder arithmetic are preserved. Its two hints constrain an initial value handoff through R5; the separate glyph barrier proved unnecessary and was removed. The meter retains only cell-pointer R2 and unselected-template-pointer R1 pins.

The centering offsets read from the ROM are4,3,3,2,2,1,1,0. Selected map entries are F0C3 for the first seven cells and F0C5 for the eighth; unselected entries are F0C7 for the first seven and FCC8 for the eighth. Template names describe this consumer's conditional choice; the broader HUD meaning remains tentative. The fill helpers' names state observed tile IDs without assuming the tiles are visually blank.
