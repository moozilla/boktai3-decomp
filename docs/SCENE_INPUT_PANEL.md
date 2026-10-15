# Scene input panel display and comparison

Reviewed cohort: 0805EA58–0805F7E0, with the next function excluded. Names are provisional; the adjacent controller identifies an inventory-like selection screen, but no new runtime screen coverage is claimed.

All 17 function extents are exact. Eleven newly recovered functions add **0.1330 percentage points** (3,212 bytes): 808 ordinary-C bytes and 2,404 bytes retaining minimized register/allocation concessions. Six previously exact functions (252 bytes) gain shared types or names without additional credit. Each new match has its own translation-unit commit. No explicit r7 bindings or handwritten instruction bodies are used.

## Display owner

`scene_input_panel.h` records a partial owner through 0x14E8. Two relocated 32-byte resource headers occupy 0x20 and 0x40. A 0x2C-byte drawable plus 0x1C-byte graphics record occupies 0x60–0xA8. Its position is at 0x7C, distinct from each keyframe widget's position at widget+0x20. The remaining display objects have a 0x60-byte stride:

| Offset | Worker name / observed role |
|---|---|
| A8 | indicator |
| 108,168 | valueA,valueB |
| 1C8,228 | prompt,mode |
| 288 | primary[16], 4×4 positions at X=112+24×column,Y=32+24×row |
| 888 | secondary[8], 4×2 positions at X=8+24×column,Y=32+24×row |
| B88 | buttons[4], X=216 and Y=32,56,80,104 |
| D08,D68,DC8,E28 | banner,compareA,category,modeCursor |
| E88,EE8,F48,FA8 | heading,valueLabel,glyph,meterGlyph |
| 1008 | digits[8] |
| 1308,1368 | numericLabel,smallDigits[4] |

EA58 loads the C091/A597 background resource, initializes layer zero, loads tile data, and copies 24 words from resource 92B3/081B+1B4 to 03005160. Five tile runs are blanked before drawing the centered meter. Its second argument is unused and its third retains full-word width.

EB0C loads and relocates CB05/5D04 and CB05/530D. It initializes the widget cohorts above, uses mode-indexed keyframe/animation/palette/sequence tables, and immediately decodes one sequence tick before placing the drawable at (8,80). Sequence commands encode frame at >>6, orientation at bits4–5, and delay weight at bits0–3. Reverse/hold flags, narrowed elapsed/index arithmetic, signed delay scaling and minimum-one delay agree with the neighboring sequence players. Only the low three packed coordinates are initialized; implicit padding is preserved by generated stores and is never read as a C value.

F0A4 positions the value label using DEB8[kind-1] plus a value and loads a table-selected meter glyph. F0E8 maps modes 4/5,1,3,2 to Y=32,56,80,104 at X=216, and hides the mode cursor for other modes.

## Highlight palette

F1DC copies bank 0x3AA (palette base+0x7540) into a 16-color buffer and resets its phase byte at +0x20. F210 cycles only color 12 through the established 50-tick color helper. F220/F278/F2CC select which of compareA,modeCursor,prompt uses a supplied palette with selector 0x8000. F324 resets all three to bank 0x3AA; its second caller argument is unused.

## Value and attribute comparison

`scene_input_compare.h` keeps a separate equipment view: signed selected slot at 0x50, primary slots[8] at 0x68 and secondary slots[8] at 0x78. F3A4 reads the selected valid descriptor and returns primary/secondary values through optional outputs. Primary descriptors are copied into a 32-byte record through 0822D808 before use. Secondary descriptors come from 0822E5F4. The query preserves the final global-pointer reload before an optional secondary descriptor copy.

F374 splits a signed value into hundreds, tens and ones using truncating division; it does not clamp or take an absolute value. F480 computes the candidate or equipped primary value, compares it with the equipped baseline, and draws three digits with equal/higher/lower glyph offsets 0/10/20. The secondary value is shown only when no candidate is supplied. Hidden groups receive tile 0x020 and their hide bit; visible groups receive tile 0x001 and visible keyframes. Coordinate-to-tile conversion preserves signed right shifts.

F63C compares three current attributes (0815A384) with three candidate attributes (0815A3C4), using the same 0/10/20 glyph-bank convention. Each group draws three digits without changing visibility flags. Exact attribute identities remain tentative.

F758 applies a small alternating Y adjustment through frame 11, leaves Y unchanged on the other phase, and reports completion after frame 15. F784 writes action 0x1CF8 and resets frame 0x1AEE. F79C hides banner D08, selects text through pointer 1A88, sets a 30×2 text window at (0,16), enables it and applies the supplied text mode. These final functions use narrow local extension views; the integrator's scene_inventory_panel.h owns the larger controller view.

## Validation and concessions

Full reviewed byte extents were checked with tools/check.py against the private ROM/ELF. Hinted functions were minimized with depin --write before promotion. EB0C retains defined pointer/constant lifetimes and low-register hints; its final scratch-register constraint keeps the last loop position pointer alive through the following X store. The initial owner copy uses a local r0 input, allowing the ordinary compiler allocation to keep the owner in r7 without binding r7. Reversing the chained coordinate clear to z=y=x=0 preserves the observed XY-before-Z packed-store order.

Complete ROM validation is deferred to the orchestrator's combined integration build. No worker full build or emulator coverage run was performed.
