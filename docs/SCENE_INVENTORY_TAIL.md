# Inventory controller tail, catalogs and completion

Reviewed **080620D4–080632AC**, all **26 functions /4,568 bytes** exact. Fourteen newly recovered bodies add **0.1530 percentage points** (3,696 bytes): 1,560 ordinary-C bytes and 2,136 bytes retaining minimized allocation concessions. Twelve existing leaves (872 bytes) receive types and descriptive names without additional credit. Full combined ROM validation belongs to the integrator. No runtime screen identity or new emulator coverage is claimed. Offsets, flag masks, sound IDs, keyword IDs and table addresses below are hexadecimal; byte counts, coordinates and counts are decimal.

The phase views are `scene_inventory_tail.h`, `scene_inventory_catalog.h` and `scene_inventory_lifecycle.h`. They preserve the established display prefix, navigation grid at 1BE4, highlight colors at 1D00, controller frame at 1AEE, action/callback words at 1CF8/1CFC, and completion-script key at 1CF4. Purchase/sale result bytes are at 1AAC/1AAD; blink frame is at 1AAE. The choice helper uses bytes 1AAA and1AAF. These fields overlap catalog/transition storage in different phases; they are not independent simultaneous buffers. The catalog builders clear the first56 entries (seven pages of eight), while the broad array view preserves the previously reviewed phase workspace extent. Script readers rely on the original ROM's list limits.

## Selection and transactions

620D4 is the normal action-selection callback. It updates comparison highlighting, lets earlier mode/page handlers consume input, handles B as cancel (soundDE), dispatches A by category, and marks a successful navigation with flag 80 plus prompt refresh. 62190 is the move-selection callback: B returns according to mode 1 versus4/5; A dispatches the category predicate. Results1/4/5 start the corresponding transition;4/5 play107, and rejection plays192. Both return zero.

62298 confirms a category reset: accepted choice playsDD and calls the category reset helper with zero; cancellation playsDE. Both restore the initial controller. The existing623C0/625E0/627C4 choice callbacks are purchase, sale and discard confirmation respectively. The swap state62324 waits for the marker transition, then swaps saved/current selections through the category inventory helper and resumes the controller. 62818 similarly waits before removing the saved slot and starting discard feedback.

62414 establishes the purchase interpretation directly. After transition completion, equipment copies a32-byte catalog definition into a descriptor, clears its bytesC/10/14 (tentative modifier fields), passes the customization hook, inserts it into the destination slot, and sets condition500. It subtracts the definition's unsigned base value at+6 from funds. Items subtract their base value and insert the catalog ID; consumables preserve the extra definition lookup, insert the ID, then subtract their base value. All cases set purchased=1, play111, and restore the controller. Catalog lookup uses `savedSelection -16 + page*8`; IDs remain signed bytes.

62634 adds the category-specific calculated sale value to the game-state halfword at 7B0, then removes the saved inventory slot. The stored halfword is reloaded as signed before the upper cap9999 is applied; preserve this order rather than treating it as a wide saturating addition. Item sales also set the corresponding bit in unlock words 750/754 before removal. All cases set sold=1 and install the sale-feedback callback. This directly supports the economic names used by earlier comparison and value-display functions.

The existing sale/discard feedback states62778/6288C play their sound on frame zero, adjust button0/button3 toward Y32/Y104, and return to their respective controllers when the sliding helper completes.

## Display driver and initialization

628D4 restores the controller, refreshes the page meter and category grid, then clears both comparison displays. 62978 hides the description banner, resets the text system and clears comparisons.

629E0 is the central display refresh. Observed flags are:

| Bits | Effect |
|---|---|
| low2 equal1 | Blink both value widgets using bit5 of the incremented byte counter |
|4 | Position/show the comparison cursor; otherwise hide it |
|8 | Show the category widget; otherwise hide it |
|10 | Enable value display; absence hides its four digits |
|20 | Enable category description; absence clears description/comparisons |
|40 | Repopulate the category grid |
|80 | Mark a selection/display change |

When either40/80 is set, the driver refreshes the selected category's value and description as enabled. It then clears those two transient bits, refreshes the page meter and advances highlight animation. This entire532-byte driver is ordinary C.

62C08 forwards its incoming mode to the established widget initializer (confirmed by the caller at 63416), initializes highlight state, and constructs five groups of three comparison glyphs. Starting positions are(64,88),(64,80),(80,96),(80,104),(80,112), advancing X by 8 for each glyph. It then initializes the marker transition and prompt pair. Packed coordinate bitfields preserve unspecified padding without reading it as a C scalar. The message-box leaf62DA8 reads keyword 73, initializes the text system/window(0,16,30,2), mode 1 and the81B text setting.

## Catalogs and script completion

62DEC builds six equipment pages from table 0824DF3C, filtering nonnegative IDs through unlock words 748/74C. It fills the remaining page withFF, reads equipment text keyword 77, and optionally resolves the string after the keyword 63 reference. 62F10 builds two item pages from0824DF6C through unlock words 750/754, fills through page 6, and reads item text keyword 69. Static table entries establish nonnegative IDs at most39/26 respectively, keeping both bit-shift branches within0..31 after adjustment.

630FC copies two consumable pages from0824DF7C without unlock filtering and fills the remaining pages withFF; keyword 61 supplies its text reference. 6300C instead reads the script's keyword 54 list, rounds its count up to an eight-entry boundary, appends keyword 52 entries, and computes the combined page count (at least1). It records the first list's page count separately and reads text references69/61. The narrow array initializers and the phase overlap must remain explicit.

63194 reads the optional completion-script key from keyword 65, defaulting to zero. 631C4 waits until frame>31 and installs631E8. That state invokes the current callback through the generic function wrapper; zero continues display refresh. A nonzero result plays119, starts the outgoing presentation transition and installs63248. After another frame>31 delay,63248 calls the optional script with two32-bit arguments (purchased, sold) and kills the task. The call metadata's lower16-bit argument count is set through a bitfield, replacing the former uninitialized scalar read of its upper16 bits while retaining the exact100-byte extent.

## Matching evidence

Every complete extent passed `check.py --target-size`; final worker records are in ignored `build/tail-final.json`. Minimized hints:62414=4,62634=7,62C08=3,62DEC=3,62F10=12,6300C=3,630FC=11. No explicitr7 binding or handwritten instruction body is used.

The permuter solved620D4's final AND-result allocation in about two minutes on one core: substituting the two direct input-global reads for a cached local yielded score 15→0 and exact188-byte equality. The same change immediately matched62190. The bounded6300C search improved325→235 without matching; three minimized hints completed it. A separate search for62C08 was queued while leaf contracts were reviewed; the exact solution instead came from a defined low-register constraint on the loop's zero temporary, allowing zero/owner to allocate naturally tor 7/r8. This avoids the argument-order change caused by binding the owner directly tor 8.
