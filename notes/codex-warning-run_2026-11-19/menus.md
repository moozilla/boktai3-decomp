# Menus, HUD and dialogue

## Run status

- Stopped at the coordinator's 01:15 PST wind-down, after finishing Save menu (0811024C). Worked 31 of 69 queued units (44.9%). The next unit is Stage results (08111C44). Named holdouts from the worked units are listed below.
- Final scan of 76 worked source files reports 38 warnings and the pre-existing volatile formatter parameter now detected by the linter. Every changed translation unit and every checked header dependent matched with the stock compiler. All 90 commit requests are confirmed, ending at 6c5f6f2e0.

## Worked units

- Choice menu (08006374), 2 → 0 warnings. Agbcc reverses the ascending argument clear and reduces it to the ROM's descending address loop. Clearing the named row arrays before obtaining the row table pointers let invariant motion produce the ROM's order. Promoted ChoiceMenu_Create and reused the shared widget, resource, position and script argument types. All three choice_menu.h dependents match. Floating item effects still use the old ChoiceResource tag.
- Status screen (0800E3E0), 9 → 1 under the original detector. Added one pre-existing site to the report through improved detection, so 2 remain. The first digit grids share the tilemap destination and cursor setup in a common macro. Both translation units and all five status_screen.h dependents match. Used the SDK's CpuFill32 for four fixed-source fills and named the dirty bits. Reading and writing a palette entry by index kept the ROM's counter and address calculation without the local pointer detour. Promoted NumericOverlay_ClearActive.

- Password decoding (0801150C), 3 → 1 warnings. The signed first byte and unsigned delimiter view match without the scanning wrapper. Shared required-input guards preserve the decoder and word-list reader. The alternate record layout now owns its payload. Promoted Password_ValidateNameGlyphs and verified the changed header dependents.

- Scene indicators (08011884), 1 → 0 queued warnings. The shared sprite priority field recovers the original early zero for the direct active-flag store. Adopted shared collider, vector and sprite resource types. The timed sentinel uses assignment, and the terrain row plus column uses an ordinary addition. Both header dependents match. A follow-up uses the existing Terrain_GetCellIndex helper for terrain sampling.

- Selection menu (08019E58), 6 → 2 live warnings (5 were queued). Direct position fields and widget arrays match, as do the three distinct item rejection cases. Shared ContactPosition and script argument records replace local copies. Four routines gained inventory names.
- Dialogue controls and animation (080308BC), 4 → 0 live warnings (3 were queued). Direct field stores recovered both message-table reset constants and the automatic-advance counter. An early return clears the split-assignment report in the empty-window path.

- Dialogue task setup (0803198C), 1 → 1 under the original detector, with a second pre-existing warning found by switch coverage. The shared MessageWindow, MessagePlaybackState and resource types replace the private layouts while preserving the complete 948-byte routine. The legacy symbol remains because its external caller is outside the assigned files.

- Dialogue text interpreter (08031E54), 5 → 0 warnings. The glyph and tag constants and playback reset use the shared fields directly. Tag parsing now uses one typed scratch record, and the decimal buffer uses an indexed clear. All four translation units and the 13 shared header dependents match. The linter now reports the pre-existing volatile formatter parameter separately.

- Scrolling dialogue controller (08032EDC), 1 → 0 warnings. Direct stores to the active flag and other initializer fields retain the original clear value across both calls. All 21 functions match.

- Scripted dialogue transitions (08033C58), 2 → 0 warnings. An indexed backdrop store and compound operations on a scoped display control value match. The display bits use SDK names. All 12 functions match.

- Modifier selection and stat previews (0803F804), 5 → 4 live warnings (4 were queued). Direct zero options and SDK CpuCopy16 match using the canonical background declarations and palette buffer. The full 4,256-byte unit matches.

- Modifier menu graphics and input (080408A4), 8 → 2 warnings. Native display array loops replaced four flag locals and a private field-pointer layout. The window destructor now uses the native menu record with direct sentinel stores. All three changed units match.

- Modifier preview and lifecycle (08041908), 2 → 2 warnings. Recovered the embedded CharacterPaletteCycle and used it in the preview update. All 99 dependents match. The asset color literals are still open.

- Countdown controller (0804D620), 1 → 0 warnings. Direct tether field stores retain the original mode transition. Separate script interval branches remove the signed pointer cast. The full controller matches.

- Stage icon display (08055B74), 5 → 1 live warnings (4 were queued). Native indicator loops clear the reveal flag, destructor zero and both initialization wrappers. The three changed units match.

- Level advance display (08056F20), 7 → 1 warnings. The particle initializer uses the native random table, direct masks and native velocity clears. Indexing the particles and using frame fields directly recovered the animation update and the destructor. All three units match.

- Scene panel presentation (08057960), 8 → 0 warnings. Native palette resource and bank arrays prevent early destination address hoisting. Background effects and overlay use the shared background display API, native particle representation and direct fade and random fields. All three units match.

- Scene panel sprite rig (08059428), 15 → 8 live warnings (11 were queued). Direct cursor masks, sprite indices, field stores and the existing shared sample operation removed seven warnings, including the rig initializer’s proved-zero site. The contact callback uses the native collider and the rise routine uses the native panel. All affected units and 57 header dependents match.

- Scene panel control and lifecycle (0805B28C), 3 → 0 warnings. The tether state uses an early return, subject spawns use the native position and displacement, and the arrival ramp writes its native fields directly. The 48-function unit matches all 6,396 bytes.

- Scene panel subjects (0805CB88), 2 → 2 warnings. The closest separate-value activation draft differs only in the second halfword load and comparison register. The reaction mask has the rig callback’s same load-order difference.

- HUD tilemaps and clock digits (0805DD18), 2 → 0 warnings. Each meter branch writes its own tile before the cursors advance. The digit hide loop uses the object stride and direct flag. All 11 functions match.

- Input marker widgets (0805E190), 5 → 1 warnings. The glyph and marker pairs use native positions and indexed widget loops. The grid formula uses its own local slot in the second band. All three changed units match.

- Input grid and navigation (0805E484), 4 → 1 warnings. Separate held and pressed decisions reproduce the ROM’s reads without a volatile view or wrapper. The pair transition uses a direct increment. Both units match.

- Inventory values and display (0805EA58), 3 → 1 warnings. The background initializer passes the selected-map array and uses SDK CpuCopy32. The item grid walks the flag array by index. Both changed units match.

- Inventory action transitions (0806055C), 20 → 0 live warnings (11 were queued). Direct frame/attribute writes match in all nine single-move variants. The swap fills arrays by index, and the destination computes a scoped column. All 11 changed units match.

- Inventory transactions and catalogs (08061C3C), 5 → 1 live warnings (3 were queued). Use the actual page-count, catalog and consumable text fields instead of byte offsets. The complete catalog builder matches.

- Message panel (08109480), 4 → 1 live warnings (3 were queued). The panel uses the shared state transition and script arguments. Transfer counters use their own arrays, and the clock snapshot uses native name and style fields. All three units and the message panel header dependent match.

- Record panel (0810D590), 2 → 2 warnings. The decimal renderer and linked-name routine retain their existing matches. The ordinary candidates change allocation and the masked flag load order.

- Selection panel (0810DA38), 12 → 2 warnings. The native widget array and window handles remove flag and dimension locals. All seven link-mode checks now use the existing query. Five transition sites use the panel’s state operation. Both units and the selection panel header dependent match.

- Record menu (0810EDC0), 5 → 2 warnings. Native indexed sprite construction and display flags clear three reports. Cursor palette uses the SDK word copy. Both changed units and six header dependents match.

- Save menu (0811024C), 1 → 0 warnings. Initial score reveal uses the existing save-state operation. The full translation unit matches.

## Open decisions

- StatusScreen_FadeAndExit (0800EC0C), blueMask. Inline 0x7C00 changes invariant motion and the palette loop's allocation. Current source keeps a single blue channel mask across both banks.
- StatusScreen_WaitForStart (0800ECD0), mask. Direct BG0 expressions choose a complemented literal for the clear and materialize the set bit again after the call. The ROM uses a single bit value for both operations. A packed control field changes the access width to a byte.

- Password_EncodeSymbols (080115A4), glyph stride. Literal increments, indexed traversal, and an array of typed glyph pairs all use an immediate Thumb add. The ROM preserves a stride register across GlyphTable_FindPair.

- SelectionMenu_DrawStatistics (0801AA14), level column. A direct literal moves the column setup after the outgoing stack arguments. Tested shared drawing helpers, a coordinate record and clamp forms.
- SelectionMenu_Initialize, pause mask. Direct bitwise forms form the global address and load the flags before materializing the bit. The ROM sets the bit value first.

- Dialogue task setup (0803198C), wait/callback union. Separate native fields and typed position match except for the callback load/store choosing r0 instead of r1. The local allocation force confirmed the one decision. Typed callbacks, independent field helpers, choice-list restructuring, and a paired default record did not recover it. Native candidate and comparison are in build/cx/search/message-window-setup.

- Dialogue task setup, resource dispatch. A direct resource lookup removes the ROM's unused display-mode read. The existing case and default share the same resource.

- Text_FormatDecimal (0803209C), volatile value parameter. Removing the qualifier folds the second read after writing the sign. Taking its address keeps the reload but adds a register move and changes the stack accesses. The original declaration remains.

- Modifier_ReadPackedValue (0803F838), packed mask. ROM loads the 6-bit mask before dispatch. Direct extraction and a shared return materialize it within or after dispatch, and the equivalent bitfield emits shifts.
- ModifierMenu_DrawSecondSourceStats (080402D0), two draw wrappers. The first wrapper sets the reference priorities for the base value and two preview values. Forcing those three diagnostic allocations reproduces the ROM, while duplicating the call leaves seven extra instructions. Removing both wrappers also exchanges four stack slots with registers. The natural state/mode branches and a typed comparison record did not match.
- ModifierMenu_DrawBackground (0804085C), draw wrapper. Ordinary switch emits the same instructions with its map index and menu pointer exchanged. Raising map index allocation priority in the diagnostic compiler alone matches. Default assignment, fallthrough and if/else forms change instructions.

- ModifierMenu_HandleOverviewInput (08041514) and ModifierMenu_HandleSelectionInput (0804164C), fixed key repeat period. Direct literals load the period after copying the saved counter instead of before. The native scratch forms and the actual input API signature leave the same ordering difference.

- ModifierMenu_UpdatePreviewAppearance (08041DA8), repeated palette color. The ROM loads the same color into two registers, one for the seven-entry fill and one for entry nine. Direct stores introduce extra copies, and sharing a single local removes the ROM reload. Indexed and bank-relative loops keep a difference.

- StageIconDisplay_TickReveal (08055C04), first-frame sound wrapper. Its loop note raises the display pointer above the count pointer in allocation. Without it, the same code exchanges r4 and r5. Shared frame-pointer and ordinary control-flow forms still exchange them.

- LevelAdvanceDisplay_StartParticle (08056F88), fourth sample cursor address/dereference. Direct cursor access shortens the function from 196 to 184 bytes and changes the table, cursor and mask homes. Shared direct, indexed and draw operations use the same wrong homes. The exact source retains the address/dereference.

- PileDriverRig_ApplyContact (08059428), reaction mask. Direct forms load the flag word before forming 127. The ROM forms 127 first. The subject callback uses the same reaction mask.
- PileDriverRig_TickRise (08059AC4), rise-to-wander wrapper. Its loop note prevents CSE from making the later arm reuse the pre-branch frame information. Suppressing only that CSE follow matches the native-parent draft. Direct counter and branch forms still combine the loads.
- PileDriverRig_TickWandering (08059DF8), turn step/table union. Splitting the values changes homes for the table, cursor, mask and step and loses four bytes. The shared sample forms change the allocation but have no stock match.
- PileDriverRig_TickApproachSubject (0805A044), target distance/table union. Separate values give the table r3 where ROM has r6. Direct signed sine and distance fields preserve this exchange, and hoisting the table changes instruction order.
- PileDriverRig_TickSecondarySprites (0805A7E4), three weight/phase unions. Separating all values keeps the size and instruction shapes. A diagnostic allocation of table, random phase pointer, phase pointer and radius gives exact bytes. Native bounded loops and explicit phase fields still choose different homes.
- PileDriverRig_InitPrimarySprites (0805AE58), graphics/table union. A separate constant table is rematerialized at each draw, and suppressing that equivalence hoists the table to the loop entry. The ROM loads it once after the phase store. The standard shared draw operations miss.

- PileDriverSubject_ApplyContact (0805CCF0), reaction mask. The direct form exchanges the flag-word load and the 127 materialization. This is the same code shape as the rig callback.
- PileDriverSubject_TickActivating (0805CFE8), index/resource union. The native combined condition removes the ROM’s second index load. Keeping two reads and using a scalar index recovers the reload, but uses r1 for its compare instead of r0.

- SceneInput_TickChoice (0805E1D0), proved-zero confirm mask. The closest native draft keeps all 160 bytes and differs in the two instructions feeding the confirm AND. CSE commutes the known-one operand and regmove copies the key word rather than the mask. Named SDK keys and native argument uses still choose that operand order.

- SceneInput_BeginPairTransition (0805E484), frame reset. Direct field stores place zero after address formation, and PRE uses the first field address for the adjacent duration address. The ROM materializes zero first and forms both addresses independently. Typed and indexed loop variants also change other instructions.

- Shop_PopulateConsumableGrid (080600DC), trailing digit visibility bit. Direct flag writes exchange early reload and induction slots across the grid. Indexed flag and digit arrays still change the loop shape or homes, unlike the corresponding item grid.

- Shop_BuildConsumableCatalog (080630FC), empty catalog fill. A direct OR literal moves the immediate after catalog address formation. A direct signed sentinel removes the ROM’s read-modify-write and changes loop allocation.

- ClockPanel_CaptureSnapshot (08109AEC), second saved-name offset. Using the literal or a separate bike-name array exchanges the two initial address homes. The local now identifies the actual record field and remains a warning.

- PanelText_DrawDecimal (0810D60C), decimal remainder wrapper. Plain arithmetic exchanges the value and tile cursor registers. Its matched decimal-rendering sibling uses the same wrapper. A diagnostic priority increase gives the ROM homes.
- RecordPanel_ShowName (0810D9B0), link mode mask. The ROM sets 0x800 before loading the two flags words. Direct masking moves it after the OR and changes the panel address scratch register.

- SelectionPanel_UpdateCursor (0817597C), first cursor frame. Direct subtraction loads the frame constant after the saved offset. The ROM loads it before the offset address and read. Allocation also changes across the function.
- SelectionPanel_GetLinkModeFlag, link-mode bit. The existing query preserves the early mask and retains its warning. Direct tests move materialization after the combined flag load.

- RecordMenu_UpdateRecords (0810F544), transition flags. Direct stores move the 1 after the callback write and exchange two address scratch registers.
- RecordMenu_Initialize (0810F678), exit flag and transition argument. Direct zeros match apart from the earlier choice conversion: the ROM builds the selection field address before subtracting five. Independent typed choice and field forms keep the opposite order.

## RANDOM TABLE

- PileDriver_InitRig (0805B0F0) now matches all 412 bytes without the proved-zero adjustment. Set table directly to gRandomTable and use the existing PARTICLE_RANDOM_READ_SAMPLE(sample, table, first) for the first halfword sample, immediately before masking with 15 to compute the turn rate. Existing sampling sites after that remain ordinary table reads. The same macro on the byte-width speed sample (capture into u8) or final vertical-step sample also independently matches. The conditional sign and duration reads change code when captured this way. Start with one real sample when applying this to another effect.
- Direct gRandomTable alone exchanges the table and mask registers. The existing statement macro supplies a sample boundary and changes the table reference weight before allocation. It is already shared across the particle and scene code. No new macro, inline or literal parameter was introduced.

## PARAMETER HELPERS

- STATUS_SCREEN_BEGIN_DIGIT_GRID, 2 call sites in 2 functions. Real operation shared by the two status digit grids. It resolves the destination tile and binds the row and digit input used by their renderer. The wrapper also affects matching.
- PASSWORD_REQUIRE, 8 call sites in 2 functions. Needed for the match. It wraps generic condition and failure control flow in the password decoder and word-list loader.
- IndicatorSprite_SetFrame, 4 call sites in 3 functions. Existing frame setter updated to take the native sprite type. Kept its existing use sites and operation.
- cellIndex, 1 call site in 1 function. Existing terrain lookup whose argument type was updated. Replaced with Terrain_GetCellIndex after the audit and rechecked the whole unit.

- Bg_EnableDisplay, 23 call sites after this change, four new calls in two routines. Existing engine operation for the background display-control shadow, reused for scene effects and overlay. The SDK bit names identify the selected layers.

- MessagePanel_SetState, 12 call sites in 9 functions. Real state-machine operation shared by the notice, dismissal and reset paths. It sets the callback, entry flag and mode together.

- SelectionPanel_SetState, 5 call sites in 4 functions. Real operation for input activation, confirmation and script launch. The cancel path writes the same fields directly because the inline removes a reload there.
- SelectionPanel_GetLinkModeFlag, 7 call sites in 6 functions. Existing link-mode query reused across the panel. It retains the original mask local and linter report.

- Save menu SetUpdate, 8 call sites in 6 functions. Existing state transition reused for the initial score reveal. It sets the callback, entry flag and mode together.

## Tools

- Literal-local now sees a declaration followed by a single direct literal assignment. Corpus scan 2,725 → 3,220 warnings of that kind. Linter suite passes all 38 tests.
- Scope try normalizes known ROM address labels and function names before distinguishing whole units from snippets. The reproducer now scores the original candidate without a duplicate definition. All 9 snippet tests pass.
- The linter now flags subtraction of a negated variable. It reports 16 existing sites across the tree after the indicator fix. The updated linter suite passes all 39 tests.
- The linter now reports identical simple if/else arms, preserving literal differences. It sees 25 pre-existing sites after the selection fix. The updated linter suite passes all 40 tests.
- Duplicate-branch detection also covers identical exiting switch arms with a default. The corpus scan increased from 25 to 29 sites and all 41 linter tests pass.
- Volatile detection now includes by-value parameters, including the final parameter in a list. Four pre-existing errors are visible across the tree. All 42 linter tests pass.
- No concession-ok annotations.
