#include "engine/video/interrupts_video.h"
#include "engine/ram.h"
#include "engine/runtime.h"
#include "engine/video/render.h"
#include "gba/gba.h"

// Provisional name: InterruptsAndVideo_Initialize

/* Install interrupt handlers and clear video, sprite and palette memory. */

extern u8 gUnk_0824FFA8[];

void IntrMain(void);

void InterruptsAndVideo_Initialize(void)
{
    DmaCopy32(3, gUnk_0824FFA8, gSerialInterruptState, 0x34);
    DmaCopy32(3, IntrMain, gInterruptCode, 0x800);
    *(u8 **)0x03007FFC = gInterruptCode;
    gVideoInterruptsEnabled = 0;
    gVBlankOccurred = 0;
    DmaFill32(3, 0, VRAM, 0x18000);
    DmaFill32(3, 0xA0, OAM, 0x400);
    DmaFill32(3, 0, PLTT, 0x400);
    REG_IE = 0x2001;
    REG_DISPSTAT = 8;
    REG_IME = 1;
}

void Mosaic_Initialize(void);

void Bg_Initialize(u32);

void Video_SetWindowBounds(u32, u32, u32, u32, u32);

void Video_SetWindowLayerMasks(u32, u32, u32, u32);

void Sprite_InitSystem(void);

void ObjShape_InitTables(void);

void Anim_ResetDefaultFlags(void);

void ScanlineEffect_Initialize(void);

void Text_LoadFontResource(void);

void SpriteOam_QueueProjectedSprites(void);

void Sprite_RenderScaledList(void);

void sub_08219624(void);

void Video_SetRenderCallbacks(u32, void *, void *, void *);

u32 InterruptsAndVideo_OnVideoUploadTask(void);

u32 InterruptsAndVideo_OnGraphicsUpdateTask(void);

void Task_SetCallbacks(void *, void *, u32);

void Task_LinkToPriorityList(void *);

void InterruptsAndVideo_InitRuntime(void)
{
    u16 interrupts;

    DmaFill32(3, 0, VRAM, 0x18000);
    DmaFill32(3, 0xA0, OAM, 0x400);
    DmaFill32(3, 0, PLTT, 0x400);
    REG_IME = 0;
    (void)REG_IE;
    REG_IE = 0;
    gVideoInterruptsEnabled = 1;
    gVideoIrqPhase = 0;
    gVideoTransferControl = 1;
    interrupts = INTR_FLAG_GAMEPAK;
    REG_IE = interrupts;
    REG_IE |= INTR_FLAG_KEYPAD;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IE |= INTR_FLAG_HBLANK;
    REG_DISPSTAT = DISPSTAT_VBLANK_INTR;
    REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;
    REG_DISPSTAT |= DISPSTAT_VCOUNT_INTR;
    REG_DISPSTAT = (REG_DISPSTAT & 0xff) | 0x4000;
    REG_IME = 1;
    REG_KEYCNT = 0;
    gSpriteAttrFlags = 0;
    Mosaic_Initialize();
    Bg_Initialize(0);
    Video_SetWindowBounds(0, 0, 0, 240, 160);
    Video_SetWindowBounds(1, 0, 0, 240, 160);
    Video_SetWindowLayerMasks(0x3f, 1, 0, 0);
    gQueuedObjSlotCount = 0;
    gSceneEntryActive = 0;
    Sprite_InitSystem();
    ObjShape_InitTables();
    Anim_ResetDefaultFlags();
    ScanlineEffect_Initialize();
    Text_LoadFontResource();
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    gSceneEffectPalettePhase = 0;
    gVBlankFrameCounter = 0;
    Task_SetCallbacks(&gVideoUploadTask, InterruptsAndVideo_OnVideoUploadTask, 0);
    Task_SetCallbacks(&gGraphicsUpdateTask, InterruptsAndVideo_OnGraphicsUpdateTask, 0);
    gVideoUploadTask.priority = 0;
    gVideoUploadTask.unk16 = 1;
    gVideoUploadTask.id = 0;
    gGraphicsUpdateTask.priority = 13;
    gGraphicsUpdateTask.unk16 = 1;
    gGraphicsUpdateTask.id = 0;
    Task_LinkToPriorityList(&gVideoUploadTask);
    Task_LinkToPriorityList(&gGraphicsUpdateTask);
}

void InterruptsAndVideo_WaitForVBlank(void)
{
    gVBlankOccurred = 0;
    while (gVBlankOccurred == 0)
        ;
}

void InterruptsAndVideo_WaitForVBlank(void);

void InterruptsAndVideo_WaitFrames(u16 frames)
{
    u16 savedInterruptMasterEnable = REG_IME;

    REG_IME = 1;
    while (frames--)
        InterruptsAndVideo_WaitForVBlank();
    REG_IME = savedInterruptMasterEnable;
}

// VBlank handler: sound, BG scroll/affine registers, HBlank DMA, link/clock ticks.

void m4aSoundVSync(void);

u32 WirelessController_IsCreated(void);

void RfuManager_CheckVBlankSync(void);

void SunClock_TickFrame(void);

void InterruptsAndVideo_OnVBlank(void)
{
    m4aSoundVSync();
    gVBlankOccurred = 1;
    if (gVideoInterruptsEnabled) {
        gDialogueScrollLatched = gDialogueScrollOffset;
        gDialogueModeLatched = gVBlankDialogueState;
        REG_BG0HOFS = gBgScrollShadow[0];
        REG_BG0VOFS = gBgScrollShadow[1];
        REG_BG1HOFS = gBgScrollShadow[2];
        REG_BG1VOFS = gBgScrollShadow[3];
        if (gBgAffineMode == 0) {
            REG_BG2HOFS = gBgScrollShadow[4];
            REG_BG2VOFS = gBgScrollShadow[5];
            REG_BG3HOFS = gBgScrollShadow[6];
            REG_BG3VOFS = gBgScrollShadow[7];
        } else {
            REG_BG2PA = gBgAffineMatrixBuffer[0];
            REG_BG2PB = gBgAffineMatrixBuffer[1];
            REG_BG2PC = gBgAffineMatrixBuffer[2];
            REG_BG2PD = gBgAffineMatrixBuffer[3];
            REG_BG2X_L = gBgAffineRefBuffer[0];
            REG_BG2X_H = (gBgAffineRefBuffer[0] & 0x0FFF0000) >> 16;
            REG_BG2Y_L = gBgAffineRefBuffer[1];
            REG_BG2Y_H = (gBgAffineRefBuffer[1] & 0x0FFF0000) >> 16;
        }

        if (gScanlineDmaMode != 0) {
            REG_DMA0CNT = 0;
            REG_DMA0SAD = (u32)gScanlineDmaSource;
            REG_DMA0DAD = (u32)gScanlineDmaTarget;
            REG_DMA0CNT = ((u32)(DMA_ENABLE | DMA_START_HBLANK | DMA_REPEAT | DMA_DEST_RELOAD |
                                 (gScanlineDmaMode != 1 ? DMA_32BIT : DMA_16BIT))
                           << 16) |
                          gScanlineDmaCount;
        } else {
            DmaSet(0, 0, 0, 0);
        }

        gVBlankUploadPending = 1;
        if (WirelessController_IsCreated())
            RfuManager_CheckVBlankSync();
        SunClock_TickFrame();
        gVBlankFrameCounter++;
    }
    *(vu16 *)0x03007ff8 = 1;
}

void m4aSoundMain(void);

void InterruptsAndVideo_OnVCount(void)
{
    if (gVideoIrqPhase == 0) {
        if (gDialogueModeLatched != 0) {
            REG_DISPSTAT &= 0xff;
            REG_DISPSTAT |= 0x9000;
            gVideoIrqPhase = 1;
        }
        m4aSoundMain();
    } else {
        vu16 *horizontalScroll = &REG_BG0HOFS;
        u8 *dialogueScroll = &gDialogueScrollLatched;
        u32 statusMask = 0xff;
        *horizontalScroll = *dialogueScroll;
        REG_DISPSTAT = statusMask & REG_DISPSTAT;
        REG_DISPSTAT |= 0x4000;
        gVideoIrqPhase = 0;
    }
}

/* InterruptsAndVideo_RunIdleHook: Unused empty hook near the frame callbacks; role tentative */

void InterruptsAndVideo_RunIdleHook(void)
{
}

/* InterruptsAndVideo_RunFrameHook: Unused empty hook near the frame callbacks; role tentative */

void InterruptsAndVideo_RunFrameHook(void)
{
}

void InterruptsAndVideo_OnUnusedInterrupt(u32 a)
{
}

void Bg_UpdateScrollShadow(void);

void ScanlineEffect_StageTransfer(void);

void Palette_UploadBuffers(void);

void ObjGfx_UploadQueuedTiles(void);

void Text_FlushTileUploads(void);

void Bg_UploadMapsAndTiles(void);

void TileAnimation_Upload(void);

void Mosaic_UploadRegisters(void);

u32 InterruptsAndVideo_OnVideoUploadTask(void)
{
    Bg_UpdateScrollShadow();
    InterruptsAndVideo_WaitForVBlank();
    ScanlineEffect_StageTransfer();
    if (gVideoUploadFlags & 1) {
        DmaCopy32(3, gOamBuffer, OAM, 0x400);
        gVideoUploadFlags &= ~1;
    }
    Palette_UploadBuffers();
    ObjGfx_UploadQueuedTiles();
    Text_FlushTileUploads();
    Bg_UploadMapsAndTiles();
    TileAnimation_Upload();
    REG_DISPCNT &= 0xe0ff;
    if (!gSceneEntryActive)
        REG_DISPCNT |= gDispCntShadow | 0x1000;
    Mosaic_UploadRegisters();
    gVBlankUploadPending = 0;
    return 0;
}

void Palette_ResetObjectAllocation(void);

void ObjGfx_ResetAllocationCounters(void);

void sub_08249238(u32);

void SpriteOam_FinalizeQueue(void);

void sub_08217C7C(void);

void sub_08217BBC(void);

void Palette_RefreshObjectBanks(void);

void Palette_ApplyBackgroundFade(void);

void sub_08217430(void);

void SpriteOam_ResetQueue(u32);

u32 InterruptsAndVideo_OnGraphicsUpdateTask(void)
{
    Palette_ResetObjectAllocation();
    ObjGfx_ResetAllocationCounters();
    sub_08249238(gSpriteRenderCallback);
    sub_08249238(gExtraRenderCallback);
    sub_08249238(gProjectedSpriteCallback);
    SpriteOam_FinalizeQueue();
    if (gGrayscaleControl) {
        sub_08217C7C();
        sub_08217BBC();
        if (gGrayscaleControl > 0)
            gGrayscaleControl--;
        if (!gGrayscaleControl)
            Palette_RefreshObjectBanks();
    } else {
        Palette_ApplyBackgroundFade();
        sub_08217430();
    }
    SpriteOam_ResetQueue(0);
    gSceneEffectPalettePhase++;
    return 0;
}
