# Scene input widgets

Reviewed range: **0805E190–0805E484**, nine functions, 756 bytes. Five new functions add **576 exact bytes (+0.0238 percentage points)**:184 ordinary C and392 register-pinned. Four existing functions account for180 bytes and receive typed contracts. All full reviewed extents match. Remaining minimized hints are two in E1D0, one in E270, and one in E394. No explicit R7 binding is used. E484 starts the next consumer and is excluded.

`scene_input.h` describes a tentative reusable widget family. A widget has flags at8 and a palette pointer at48, and occupies60 bytes. A pair contains a20-byte copied/relocated resource header followed by widgets at20 and80. Position vectors occupy eight aligned bytes; the grid helper updates only X/Y. Global input5260 at byte offset2 supplies newly pressed key bits.

| Address | Name | Bytes | Contract |
|---|---|---:|---|
| 0805E190 | SceneInput_BeginChoice | 64 | Show the widget, narrow and store the initial selection byte, and choose keyframe4B for nonzero or4C for zero. Reset an optional external blink byte. |
| 0805E1D0 | SceneInput_TickChoice | 160 | A hides the widget and returns its selection byte; B hides it and returns0. A takes priority when both are pressed. With neither, Right toggles a nonzero selection and Left toggles zero; a toggle plays songDC and selects4B/4C. Update the optional palette blink and return-1 while pending. |
| 0805E270 | SceneInput_InitGlyphPair | 128 | Copy/relocate resourceCB05:530D, initialize two widgets with keyframe7F and common parameters31,0,0,60. Their positions start at(0,0,0) and(0,8,0). |
| 0805E2F0 | SceneInput_DestroyGlyphPair | 28 | Remove the two animation widgets. |
| 0805E30C | SceneInput_MapGlyphIndex | 16 | Map zero to7F and any nonzero signed input to input+80, preserving arithmetic width. |
| 0805E31C | SceneInput_SetGlyphPair | 120 | Independently hide each widget for a negative value. For nonnegative values show it and load the mapped keyframe, narrowed to16 bits, with mode1. |
| 0805E394 | SceneInput_GetGridPosition | 104 | Map the first index group to a4-column grid at(112,32), the next group to(8,32), both with24-pixel spacing. Special indices24/25 use(216,104)/(216,80); later indices use(216,32). |
| 0805E3FC | SceneInput_InitMarkerPair | 108 | Copy/relocate resourceCB05:5D04 and initialize two widgets at the same zero XYZ position, with keyframe0 and common parameters31,0,0,60. |
| 0805E468 | SceneInput_DestroyMarkerPair | 28 | Remove the two marker animation widgets. |

The choice API expects a Boolean selection byte, but preserves the original byte arithmetic `1-selection` and truncation for other inputs. Confirming zero and cancelling both return0; callers must interpret that shared result. The palette effect writes through widget.palette+12 and increments the supplied counter through0816381C. It runs on pending updates, including updates without directional input, and is skipped by early confirm/cancel returns.

Grid indices0–15 occupy the first grid and16–23 the second. Negative indices also follow the first branch, including signed right-shift behavior; the helper does not clamp them. Other Z/padding fields are retained. Both pair initializers preserve the unused high halfword of the packed position's second word. The resource and widget names describe observed roles without asserting the identity of the surrounding screen.
