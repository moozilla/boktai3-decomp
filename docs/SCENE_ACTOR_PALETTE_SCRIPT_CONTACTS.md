# Scene actor palette, script properties, and contact lists

This pass follows the registry code contiguously from **08071E00**. It distinguishes three observed sections and preserves unfinished bodies privately. Names below are provisional descriptions of behavior; no runtime coverage or game-object identity is claimed.

The frozen exact set contains **27 changed translation units / 3,748 reviewed bytes**, including **17 newly matched bodies / 3,284 bytes (+0.1360 percentage points)**. New ordinary C contributes 912 bytes; new code retaining minimized register concessions contributes 2,372 bytes. The other 464 reviewed bytes were already matched and receive corrected contracts and names without new credit. Parent integration owns name promotion, canonical prototype reconciliation, and the combined ROM build.

## Palette transitions: 71E00–72264

The manager at020004A8 embeds two B4-byte palette transitions, at64 for actor category10 and118 for category23. Each transition contains16 packed RGB15 colors at0, remaining frame count20, destination bank22, signed Q5 accumulators for red/green/blue at24/44/64, and signed byte increments at84/94/A4. `scene_actor_palette.h` records this narrow phase view.

71E00 begins a32-step transition. Arguments are destination bank, source bank, an unused third value, actor category, and a fifth value controlling manager flag20000. The routine sets manager flag2000. When an existing transition is active and its remaining count differs from32, it uses the currently generated colors as the new starting colors; otherwise it uses the source palette bank. Each increment is destination channel minus starting channel, and each accumulator starts at the channel multiplied by32. The fifth argument's higher-level meaning remains unconfirmed. The call to704A4 passes the category even though that callee ignores its incoming value; the typed formal records that actual ABI.

71F50 updates an actor's category transition and attaches the generated palette. Category flags4000/8000 prevent more than one advancement per category during a manager frame. Each step adds the signed increments to the Q5 accumulators, packs their shifted channels, decrements the remaining count, and marks that category advanced. Actor flags2CC masked by02008000 suppress palette attachment. Drawable kind0 stores the palette pointer at38; the other drawable stores it at68. An actor with flags2D4 bit40, flags2D0 bit20000, or nonpositive health is unavailable. The availability helper permits NULL, but the subsequent active-transition path assumes a non-NULL actor.

72124 reports completion using the presence of affected living actors and engine flags4800; it does not directly inspect the transition counters.72188 clears manager flags2000/20000 when no relevant actors remain.72204 binds an existing transition while it still has steps remaining, copies the actor's palette offset to the drawable, and installs the generated color array. All **five functions / 1,124 reviewed bytes** in71E00–72264 now match, including the new336-byte constructor and468-byte application. The constructor retains27 minimized hints (57→27); application retains32 (78→32), with no explicit r7 binding. Signed RGB shifts are arithmetic; subsequent channel packing casts to unsigned before left shifts, preserving defined C behavior for negative intermediates.

## Script properties: 72264–72854

All **eight functions / 1,520 bytes** in this subsection now match. Four new bodies add1,376 bytes; four existing144-byte helpers receive shared manager contracts.

72264 reads actor ID from keyword`n`, finds that registry actor, and reads two optional values after keyword`r`, defaulting to0. It writes control-tail bytes458 and45A. Their higher-level meanings remain unknown.

722C0 reads actor ID`n` and position`p`. It stores three16-bit coordinates, derives a cell from signed X/Z shifted by8, and rejects negative or out-of-map coordinates to cell0. It prefers a dynamic map override over the base cell. The low nibble of the cell's first byte supplies height in units of256; high-nibble types1/2 subtract fractional Z/X. The routine stores the requested height relative to ground at2EA, records the ground height at2BE, and projects the actor's Y to that ground. This276-byte body matches in ordinary C after removing its temporary barrier.

723D4 reads actor ID from uppercase`N`. Missing property values default to-1 and leave their destination unchanged. Observed keyword groups are:

| Keyword | Updated fields |
|---|---|
|`e`|rangeEC/copyEE, unsigned squaresF8/FC, behavior byteEA|
|`a`|four entries each at B4,BC,CC,C4, interleaved per entry|
|`A`|base healthB0, current health9C clamped to at least0, bytes9E/9F/A0, wordsA2/A4; object class1 also receives base health divided by4 at5B6|
|`l`|wordsA6/A8; object class1 also receives word416|
|`n`|control radius422, its unsigned square424, parameter420|
|`R/F/I/D`|zero-extended16-bit values stored as words at1EC/1F0/1F4/1F8|
|`G`|five optional words beginning1FC|
|`K`|wordB2, default2|

The uncertain properties are deliberately named by offsets in `scene_actor_script.h`. This928-byte body retains only the first squared-range destination pointer's r2 hint after depin (3→1 hints); it contains no instruction bodies. The narrowed radius squares are well defined because the multiplicands are16-bit unsigned values.

72774 sets or clears actor flags2D0 bit04000000 from keyword`f`, after resolving ID`n`. Its two minimized hints retain address lifetimes and prevent merging the two stores.727C4/727EC set/clear the manager's low16 flag bits from keyword`s`;72814 returns manager field62 or0;72830 sets manager mode30 from keyword`l`, default3.

## Terrain reaction boundary: 72854–72BA8

72854 is a separate752-byte actor-list terrain reaction routine. It remains private/unmatched in this pass. The following12-byte72B44 manager getter was already matched and now returns the typed registry manager.72B50 dispatches a descriptor's two-word payload atC to08232F68 for descriptor kind0..3, only when engine flags4800 are set. Its88-byte body is newly exact ordinary C. The descriptor's eventual UI/gameplay interpretation remains unconfirmed.

## Contact list prefix: 72BA8–72F4C

`scene_actor_contacts.h`, coordinated with the neighboring worker, supplies the shared manager and node prefix; `scene_actor_contact_list.h` supplies this prefix's API and narrow distance-gate view. The manager has count1C, frame20, resource24, and head28. Nodes have object0, flags4, kind5, cooldown6, owner8, and nextC. The count comparison in72BF0 is signed and limits registration to **six nodes**. The neighboring header owner is correcting the shared count type to signed.

72BA8 clears count/frame/field18/head. Its observed POP-r0/BX-r0 epilogue has no meaningful C return contract. A bounded attempt to express the return link with `__builtin_return_address(0)` failed the full28-byte check and emitted an extra stack load; it is preserved only as a private trial. No caller is authorized to use a fabricated status from this void helper.

72BC4 rejects a node with flag2, nonzero cooldown, or kind1 while the tether actor cannot begin interaction. It retains one minimized r1 hint to prevent the compiler from combining the byte flag and halfword cooldown reads into a word mask.72BF0 rejects missing/full managers and duplicate node pointers, initializes a node with active flag1, and prepends it.72C40 unlinks a matching node and decrements count.72C80 finds the first node of a kind.72CB0 tests whether a given node is registered and active.72CF0 returns the first active node's object whose first word equals the requested nonzero16-bit ID; it returns the object, not the node.

72E08 explicitly releases an active matching node: clear flag2, clear owner, cooldown240.72E50 computes the gate at actor+DC+24 for the closest candidate. If node-kind bits6 are absent it disables the gate. Otherwise an absent threshold/mode or actor byteEB bit4 enables it; mode2 or a metric greater than the threshold disables it; the other paths enable it. Its full six-argument ABI is recorded. Depin reduced6→2 hints, retaining actor r5 and gate r4 only. These new registration/query/release/gate bodies are exact over their full reviewed extents.

72D34's full212-byte body is newly exact, with18 minimized hints (39→18) and no explicit r7 binding. Every30 manager frames it releases invalid owners with cooldown120, including unavailable/dead owners, owner flags2CC bits3, or absent owner flags2D4 bit1000. One invalid-owner path revisits the same node after clearing its ownership bit before advancing; the draft preserves that observed control flow.72E98 selects one of four player slots from the contact object's byte8, then checks either scene-mode/action conditions or a status flag mask. Its full180-byte body is newly exact, with12 minimized hints (24→12) and no explicit r7 binding. Mode0 accepts an active player whose scene mode equals the condition; otherwise condition0 checks actionFlags bit4 and condition1 checks bit40. Mode1 tests the requested mask against statusFlags20.72F4C is the following independent **2,800-byte** contact evaluator ending73A3C;73500 lies inside this function and is not a valid boundary. The adjacent worker owns73A3C onward.

## Verification and searches

All27 frozen TUs were checked against explicit full reviewed sizes after formatting. The initial23 results are in the private checkout's `build/actor-palette-script-contact-final.json`; followups use `build/palette-begin-final.json` (71E00), `build/palette-apply-final.json` (71F50), `build/contacts-player-final.json` (72E98), and `build/contacts-owner-final.json` (72D34). No new boundary or byte-denominator change is introduced here. No worker full build was run.

The previous route-family gaps6D184/6D438/6D740 remain uncredited. A bounded60-minute6D184 search finished without improving weighted score40 (eight scratch-register differences). The valid71F50 search improved630→463 before subsequent manual lifetime shaping closed the full body; its retained hints were then minimized.6D740 has a valid combined improvement to measured score778/988 bytes (permuter750); candidates that read an uninitialized introduced variable were rejected. A bounded search continues on its frozen base. Raw fuzzy percentage alone was insufficient to compare these near matches, so weighted instruction scores and source validity are reviewed together. The source, search directories, and check artifacts remain private and are not generated-code commits.
