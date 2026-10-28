# Shared IWRAM object layouts (2026-10-27)

Tags: #matching #tooling #ram

## 030042A0: render camera

`gCamera` in `include/render.h` is a 0x14-byte `CameraRenderState`, distinct
from the active camera task pointer. The merged declarations and ROM agree on
signed position X/Y/Z, screen offsets at +08/+0A, and projection mode at +12.
The position tail at +06 is copied; its independent meaning is unresolved.
+0C and +10 are initialized to 1 and 0; +0E remains padding. Semantic names
are provisional. Forging-book views called +04 Y; they now select position Z
and retain their unsigned reads. Whole-position copies preserve eight bytes.

All 30 direct C consumers pass `tools/check.py` against the ROM. Before/after
agbcc assembly is identical (allowing the address-symbol rename) in default
and NONMATCHING modes for 283 direct and header-dependent units. No protected
source file changed. No full build was run; batch integration owns that check.

Tool gap: a struct migration mode with per-file field mappings and a protected
file exclusion list would avoid manual baseline collection and repeated checks.

## 03004C30 / 03004CC0: background records

`gBgRecords[4]` shares one 0x30-byte `BackgroundLayer` in `include/render.h`.
The storage map agrees with the ROM and the outside audit: +00/+02 carry
resource mode/layer, +10/+12 scale or overlay tile coordinates, +14/+16 origin,
+18/+1A dimensions, +1C/+1E copied dimensions, +20/+22 scroll, and +24/+28/+2C
resource, map-data and output pointers. Names remain provisional where modes
reuse fields. Signed dimension/scroll fields follow signed ROM readers;
unsigned uploader reads retain explicit casts. The initializer's pointer words
now have pointer types. Existing pause-menu aggregate copies still match.

03004CC0 is record 3, and its consumers now select `gBgRecords[3]` rather than
declaring another object. The scene overlay retains its literal-address reload
constraints through the common type. `Bg_UpdateScrollShadow` retains raw +B0
and +B2 reads: spelling those as fields produces 172 bytes versus the ROM's
180 by sharing a record base, as independently witnessed in the research kit.

All 28 changed direct C consumers pass per-unit ROM checks. The protected
`scanline_presentation.c` and `sub_08110C9C.c` remain untouched with their old
address declarations: their default builds match and their draft mismatches
are unchanged. Before/after assembly agrees in both modes for all 265 units
reached by the original declarations and headers. A protected-file exclusion
and field-remapping support in ramlayout would make this migration faster.

- **03000000 `gIwramCode` (unified):** Startup instruction buffer, unsized because its copied extent comes from ROM symbol subtraction; ramlayout proves unchanged startup code. New-header probe requires a precreated header skeleton.

- **03001110 `gVideoIrqPhase` (unified):** Tentative two-phase video/sound IRQ latch toggled in 08214144; both consumers retain identical compiler output.

- **03001118 `gVideoUploadTask` (struct):** Static scheduler task identified by its video-upload callback and priority 0. RuntimeTask preserves the scheduler header; +16 remains semantically unresolved. InitRuntime matches with identical compiler output in both modes.

- **03001130 `gGraphicsUpdateTask` (struct):** Static scheduler task identified by its graphics-update callback and priority 13, sharing RuntimeTask with the video-upload task. InitRuntime still matches in both modes.

- **03001148 `gObjGfxSlots` (struct):** 128 eight-byte OBJ tile-upload slots, proven by allocator limit and next address 03001548; both consumers exact in both compiler modes. Inventory extent corrected from one entry to 0x400 bytes.

- **03001148 `gObjGfxSlots` (struct):** Correct the header-adoption counter signedness conflicts from the preceding commit: explicit s16 reads preserve the original unsigned shared declarations. Both units now pass ROM checks and match original compiler output in both modes.

- **03001548 `gBgMapCacheKey` (unified):** Last compressed BG map resource key, compared before LZ77 unpacking (tentative cache name); ramlayout before/after checks passed.
- **0300154C `gSavedDispCnt` (unified):** DISPCNT saved by Video_SaveDisplayControl; ramlayout before/after checks passed.
- **0300154E `gSavedWindowShadow0` (unified):** Saved window shadow slot 0; ramlayout before/after checks passed.
- **03001550 `gSavedWindowShadow3` (unified):** Saved window shadow slot 3; ramlayout before/after checks passed.
- **03001552 `gSavedWindowShadow2` (unified):** Saved window shadow slot 2; ramlayout before/after checks passed.
- **03001554 `gSavedWindowShadow1` (unified):** Saved window shadow slot 1; ramlayout before/after checks passed.
- **03001556 `gSavedWinIn` (unified):** Saved WININ register; ramlayout before/after checks passed.
- **03001558 `gSavedWinOut` (unified):** Saved WINOUT register; ramlayout before/after checks passed.
- **03001560 `gTextTileUploadIndices` (unified):** Queued text tile indices; bounded storage precedes its count, original guard permits index 256; ramlayout before/after checks passed.
- **03001660 `gTextTileUploadCount` (unified):** Number of queued text tile DMA uploads; ramlayout before/after checks passed.

- **03001664 `gFontResource` (struct):** Shared 12-byte font header has two glyph counts and two relocated data addresses; narrow/wide names tentative. All seven consumers exact and unchanged in both compiler modes.

- **03001668 `gRootTask` (struct):** Scheduler root task uses the shared RuntimeTask layout. Identity follows TaskSystem_UpdateRoot callback; initializer exact in both modes.

- **03001680 `gMessageQueueBufferIndex` (unified):** Active double-buffered message queue index, toggled by MessageQueue_SwapBuffers; ramlayout before/after checks passed.
- **03001684 `gTrapDispatchStarted` (unified):** Set by Trap_UpdateProbeContacts and reset when boxes are loaded; ramlayout before/after checks passed.

- **03001690 `gSerialByteReceiveWrite` (unified):** Write cursor of the 64-byte serial receive ring; ramlayout before/after checks passed.
- **03001694 `gSerialByteReceiveRead` (unified):** Read cursor of the 64-byte serial receive ring; ramlayout before/after checks passed.
- **03001698 `gSerialByteReceiveCount` (unified):** Queued receive-byte count; ramlayout before/after checks passed.
- **0300169C `gSerialByteSendWrite` (unified):** Write cursor of the 64-byte serial send ring; ramlayout before/after checks passed.
- **030016A0 `gSerialByteSendRead` (unified):** Read cursor of the 64-byte serial send ring; ramlayout before/after checks passed.
- **030016A4 `gSerialByteSendCount` (unified):** Queued send-byte count; ramlayout before/after checks passed.
- **030016A8 `gSerialByteReceiveBuffer` (unified):** 64-byte receive ring; independent from the adjacent send ring; ramlayout before/after checks passed.
- **030016E8 `gSerialByteSendBuffer` (unified):** 64-byte send ring with its own literal base; prior interior classification was adjacency; ramlayout before/after checks passed.
- **03001728 `gSerialQueuedSendWord` (unified):** Queued multiplayer send word, moved into the active word when ready; ramlayout before/after checks passed.
- **0300172A `gSerialActiveSendWord` (unified):** Current multiplayer send word, with protocol status bits; ramlayout before/after checks passed.
- **03001738 `gRfuApiBuffer` (unified):** RFU API workspace passed to rfu_initializeAPI with size 0xE64; ramlayout before/after checks passed.

- **030025A0 `gSceneLifecycleTask` (struct):** Scene lifecycle task shares the scheduler header and names its two trailing state/counter words. Initializer exact and identical in both modes.

- **030025F8 `gSceneBodyRegistry` (struct):** Shared registry task pointer and pointee layout with first/last list heads at +18/+1C. All six consumers exact and unchanged in both compiler modes.

- **030025FC `gSceneStimulusState` (struct):** Shared 0xD8-byte pointee: eight 16-byte stimuli at +18, sixteen exclusion ids at +98. All eleven consumers exact in both modes. ID keeps unsigned aging semantics and explicit signed interpretations in posting/overlap tests; signed storage would fold the original load/OR/store into a -1 store.

- **03002608 `gTransferSavedRuntimeFlag` (unified):** Saves 0300546C while save transfer suspends the runtime; semantic name tentative; ramlayout before/after checks passed.
- **030035B0 `gSolarMeasurementPhase` (unified):** Solar timer-interrupt measurement phase; ramlayout before/after checks passed.
- **030035B4 `gSolarInterruptCounter` (unified):** Counter incremented during active sensor interrupts; ramlayout before/after checks passed.
- **030035B8 `gSolarLastInput` (unified):** Last GPIO light-input bit observed by the timer handler; tentative name; ramlayout before/after checks passed.
- **030035BC `gSolarGpioShadow` (unified):** GPIO output/control shadow used by solar timer phases; ramlayout before/after checks passed.
- **030035C0 `gSolarEnabled` (unified):** Software sensor-enable flag, set by Solar_Enable and cleared by Solar_Disable; ramlayout before/after checks passed.
- **030035C6 `gRtcProtected` (unified):** RTC GPIO protection flag set by Rtc_Protect and cleared by Rtc_Initialize; ramlayout before/after checks passed.

- **030035C8 `gSoftDoubleNaN` (struct):** libgcc double NaN object shares its 20-byte number layout in libgcc_numbers.h. Both compiler modes retain identical instructions; only internal long-branch label numbers differ. Per-unit ROM check exact; layout scanner misses the typedef-based block declaration.

- **030035E0 `gSoftSingleNaN` (struct):** libgcc single-precision NaN object shares its 16-byte number representation; both compiler modes preserve instructions and the per-unit ROM check is exact.

- **030035F0 `SoundMainRAM_Buffer` (unified):** MP2K RAM instruction buffer copied with CpuCopy32 size 0x400; next script procedure word is a separate object; ramlayout before/after checks passed.

- **030039F0 `gScriptEntryProcedure` (unified):** Script entry procedure selected by Script_SetEntryProcedure; separate from the preceding MP2K buffer; ramlayout before/after checks passed.
- **03003A04 `gVideoUploadFlags` (unified):** Video upload dirty flags; bit 0 requests OAM upload; ramlayout before/after checks passed.
- **03003A08 `gSceneEntryActive` (unified):** Set while scene-entry script runs; suppresses part of video upload task; ramlayout before/after checks passed.
- **03003A14 `gRfuIrqSlots` (unified):** Interior view of interrupt dispatch table entry 1, passed as RFU handler slots; not an independent allocation; ramlayout before/after checks passed.
- **03003A44 `gVBlankOccurred` (unified):** VBlank wait latch, volatile for interrupt polling; ramlayout before/after checks passed.
- **03003A48 `gDialogueScrollOffset` (unified):** Dialogue scroll/tick offset copied into the video IRQ shadow; tentative name; ramlayout before/after checks passed.
- **03003A4C `gDialogueModeLatched` (unified):** VBlank-latched dialogue state controlling the two-phase video IRQ; tentative name; ramlayout before/after checks passed.
- **03003A00 `gUnk_03003A00` (skipped):** Protected status_screen_fade.c inherits its declaration from status_screen.h; restoring the protected source after the header rename leaves the old name undeclared. Object fully rolled back.

- **03003A60 `gInterruptCode` (unified):** IWRAM interrupt dispatcher copy, 512 words copied from IntrMain; ramlayout before/after checks passed.
- **03004260 `gVBlankUploadPending` (unified):** Set by VBlank and cleared when the video upload task finishes; tentative name; ramlayout before/after checks passed.
- **03004264 `gVBlankFrameCounter` (unified):** Counter incremented after VBlank clock/link updates; ramlayout before/after checks passed.
- **03004268 `gDialogueScrollLatched` (unified):** Dialogue scroll offset latched in VBlank and read as a byte by the secondary video IRQ; ramlayout before/after checks passed.
- **0300426C `gVideoInterruptsEnabled` (unified):** Gates video register updates and link/clock work in VBlank; ramlayout before/after checks passed.
- **0300427C `gVideoTransferControl` (unified):** Runtime transfer-control flag initialized to 1 and cleared around save/wireless transfer; tentative name; ramlayout before/after checks passed.
- **03004270 `gUnk_03004270` (skipped):** Protected status_screen_fade.c inherits its old HBlank DMA declaration; the header rename makes the restored source fail compilation. Rolled back.
- **03004274 `gUnk_03004274` (skipped):** Protected status_screen_fade.c inherits its old HBlank DMA declaration; the header rename makes the restored source fail compilation. Rolled back.
- **03004278 `gUnk_03004278` (skipped):** Protected status_screen_fade.c inherits its old HBlank DMA declaration; the header rename makes the restored source fail compilation. Rolled back.

- **030042C0 `gSpriteListHeads` (unified):** Two sprite render-list heads, selected by gRenderListIndex; pointees retain subsystem views; ramlayout before/after checks passed.
- **030042C8 `gMainSpriteListHeads` (unified):** Two main/animation sprite-list heads; protected ARM source retains its address declaration; ramlayout before/after checks passed.
- **030042D0 `gDrawableListHeads` (unified):** Two drawable/effect render-list heads, selected by gRenderListIndex; ramlayout before/after checks passed.

- **030042EC `gSpriteTileResource` (struct):** Loader and lookup views merged: tile count +4, entry count +6, relative tile-data offset +8, eight-byte entries from +C. All three direct consumers exact in both modes.

- **03004300 `gAnimationResource` (struct):** Shared animation bank header has count +4, relative section offsets +C/+10/+14 and 12-byte entries from +18. Loader and lookup remain exact in both modes.

- **03004310 `gSpriteAffineParams` (struct):** Shared 32-entry affine array has u8 angle, s8 X/Y scales and an unresolved tail byte. All six converted consumers exact in both modes; untouched ARM source also matches in both modes under its original address declaration.

- **03004FE0 `gBgPaletteBank1` (unified):** Interior background palette bank 1, at gBgPaletteBuffer +0x20 bytes; ramlayout before/after checks passed.
- **03005000 `gBgPaletteBank2` (unified):** Interior background palette bank 2, at gBgPaletteBuffer +0x40 bytes; ramlayout before/after checks passed.
- **03005020 `gBgPaletteBank3` (unified):** Interior background palette bank 3, at gBgPaletteBuffer +0x60 bytes; ramlayout before/after checks passed.
- **030050E0 `gBgPaletteBank9` (unified):** Interior background palette bank 9, at gBgPaletteBuffer +0x120 bytes; ramlayout before/after checks passed.
- **03005140 `gBgPaletteBank12` (unified):** Interior background palette bank 12, at gBgPaletteBuffer +0x180 bytes; ramlayout before/after checks passed.
- **03005164 `gBgPaletteBank13Color2` (unified):** Interior view starting at background palette bank 13 color 2; ramlayout before/after checks passed.
- **03005174 `gBgPaletteBank13Color10` (unified):** Interior background palette bank 13 color 10; ramlayout before/after checks passed.
- **03005176 `gBgPaletteBank13Color11` (unified):** Interior background palette bank 13 color 11; ramlayout before/after checks passed.
- **0300517C `gBgPaletteBank13Color14` (unified):** Interior background palette bank 13 color 14; ramlayout before/after checks passed.
- **03005180 `gBgPaletteBank14` (unified):** Interior background palette bank 14, at gBgPaletteBuffer +0x1C0 bytes; ramlayout before/after checks passed.
- **030051A0 `gBgPaletteBank15` (unified):** Interior background palette bank 15, at gBgPaletteBuffer +0x1E0 bytes; ramlayout before/after checks passed.
- **03005160 `gUnk_03005160` (skipped):** Existing scene_input_panel.h byte-array and status_screen.h halfword-array exports conflict when render.h exposes one shared bank type; ramlayout reports 123 compiler failures and writes nothing. Other palette aliases are unaffected.

- **03005260 `gUnk_03005260` (skipped):** Protected selection_menu.c inherits the key declaration through shared UI headers. Restoring the source after renaming those headers makes the old name undeclared; the attempted migration was fully rolled back.
- **03005270 `gHardwareKeys` (unified):** Raw active-low KEYINPUT sampled before per-player input records are updated; ramlayout before/after checks passed.
- **03005278 `gLcgState` (unified):** Independent 32-bit linear-congruential state, initialized to 123456 and advanced by Random_Next15; ramlayout before/after checks passed.
- **030052F0 `gResourceRegistryCount` (unified):** High-water count of resource-registry entries, capped at 32; ramlayout before/after checks passed.
- **0300530C `gSerialMultiplayer1Word` (unified):** Last word sampled from SIOMULTI1 by the byte-transfer interrupt; ramlayout before/after checks passed.
- **03005310 `gSerialPeer1Interrupts` (unified):** Counter advanced by the SIOMULTI1 byte-transfer handler; tentative name; ramlayout before/after checks passed.
- **03005314 `gSerialInitialized` (unified):** Set to 1 by Serial_InitMultiplayer; no semantic reader observed here; ramlayout before/after checks passed.
- **03005318 `gSerialPeer0Interrupts` (unified):** Counter advanced by the SIOMULTI0 byte-transfer handler; tentative name; ramlayout before/after checks passed.
- **03005320 `gSerialMultiplayer0Word` (unified):** Last word sampled from SIOMULTI0 by the byte-transfer interrupt; ramlayout before/after checks passed.
- **03005330 `gSerialQueuedPacketCount` (unified):** Number of queued four-halfword multiplayer packets; ramlayout before/after checks passed.
- **03005350 `gSerialPacketQueue` (unified):** Four queued packets of four halfwords, with ring indices masked by 3; ramlayout before/after checks passed.
- **03005380 `gSerialPacketReadIndex` (unified):** Current read index in the four-entry multiplayer packet ring; ramlayout before/after checks passed.
