# Inventory transitions and confirmation states

Reviewed state setup cohort: `08061540–08061C3C`, seventeen functions, 1,788
bytes. Four new functions add **0.0354 percentage points** (856 bytes):596
ordinary-C bytes and260 bytes retaining ten minimized allocation hints. Thirteen
existing functions gain typed contracts/names without new credit. Every complete
extent was checked; no specific runtime screen coverage is claimed.

The control owner embeds the established input-display prefix. Text pointers at
1A8C/1A94/1A98 and choice value/blink bytes at1AAA/1AAF are now named in
`scene_inventory_control.h`. Clock mode at1AAB is a distinct byte. Category0/1/2
selects equipment/items/consumables. The catalog still has a separate phase view
because its storage overlaps the transition and navigation state.

## State setup

| Entry | Observed setup and next callback |
|---|---|
| 61540 | Flags40 (41 when pageCount>1), cache a free destination, clear action glyph, then mode menu61DA0 |
| 6158C | Selection setup0, OR flags10/1, grid flags1C, recompute prompt |
| 615D8/61600 | Selection setup1, grid flags1E, recompute prompt |
| 61628 | Save selection, flags2C, copy comparison position to category position, grid flags1E, then62190 |
| 6167C | Save selection, flags14, purchase confirmation with choice1, then623C0 |
| 61780 | Save selection, flags14, sale confirmation with choice0, then625E0 |
| 6186C | Flags0, exit confirmation with choice1, then62298 |
| 61914 | Save selection, flags4, removal confirmation with choice0, then627C4 |
| 619D4 | Flags80, save selection and choose cached destination, start category catalog move, then62414 |
| 61A40 | Flags80, start category special move, then62634 |
| 61A98 | Flags80, start category swap, then62324 |
| 61AF0 | Flags80, start category removal move, then62818 |
| 61B48 | Selection setup2, OR flags90/1, grid flags1C, recompute prompt |
| 61B94/61C04 | Selection setup2, OR flags80, grid flags1E, recompute prompt |
| 61BCC | Selection setup3, OR flags80, grid flags1E, recompute prompt |

Confirmation setup preserves all four arguments supplied to
`SceneInputPanel_ShowDescription`. Its actual body uses only owner and mode,
reading its text pointer at1A88; the supplied text/item arguments are unused.
Their declarations now retain those unused ABI arguments. Purchase setup reads
catalog IDs, sale setup reads owned IDs, and exit/removal use-1 for item-like
categories. Exit/removal category2 deliberately supply the item text pointer,
matching the original. Generic callback storage uses an explicit function-pointer
cast where a callback returns a status; its declaration preserves that return.

## Move builders

The root transition cohort covers `60660–61000` plus its adjoining swap draft.
Exact builders cover equipment/item/consumable removal moves, special moves and
catalog moves;60864 classifies equipment moves. They hide the selected source
widget, collect its keyframe and source grid position, and invoke the shared pair
transition with duration12 and sound38A. Removal targets grid position24; special
moves target26; catalog moves target the current selection. A second disabled
entry has X/Y/Z cleared, without reading position padding. Both attribute words
are373, although the transition initializer does not consume that argument.
Equipment frames are kind+240, consumable frames ID+289, and items use their
keyframe lookup. Equipment placeholders use393, item placeholders337; the
consumable special/removal placeholder is289 and catalog placeholder337.

60864 returns1 for a different ordinary source/destination with at least one
occupied equipment slot in mode1. A special source selected again restores the
cached destination and returns4; selecting an empty ordinary slot also returns4.
The four existing free/assigned-slot queries retain an unused owner argument where
callers pass it, and expose their signed-1 failure result accurately.

The260-byte equipment swap6055C remains a99.2% scratch candidate; consumable
swap60A70 and item swap60DB4 remain assembly. No draft receives exact credit.
Long background permuter searches use isolated source/symbol snapshots for6055C,
60A70 and the earlier E484/E5E4 gaps. Candidates require source review, a complete
extent check and combined validation before acceptance. The old E484 gap still
limits the continuous exact span; this cohort does not claim to close it.
