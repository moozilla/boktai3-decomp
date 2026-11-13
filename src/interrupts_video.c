#include "interrupts_video.h"
#include "ram.h"
#include "runtime.h"
#include "render.h"
#include "gba/gba.h"

// Provisional name: InterruptsAndVideo_Initialize

/* Install interrupt handlers and clear video, sprite and palette memory. */

extern u8 gUnk_0824FFA8[];

void IntrMain(void);

void InterruptsAndVideo_Initialize(void)
{
    struct VideoDmaRegisters *dma = (struct VideoDmaRegisters *)REG_ADDR_DMA3SAD;
    u32 fill;
    dma->source = (u32)gUnk_0824FFA8;
    dma->destination = (u32)gSerialInterruptState;
    dma->control = 0x8400000D;
    dma->control;
    dma->source = (u32)IntrMain;
    dma->destination = (u32)gInterruptCode;
    dma->control = 0x84000200;
    dma->control;
    *(u8 **)0x03007FFC = gInterruptCode;
    gVideoInterruptsEnabled = 0;
    gVBlankOccurred = 0;
    fill = 0;
    dma->source = (u32)&fill;
    dma->destination = 0x06000000;
    dma->control = 0x85006000;
    dma->control;
    fill = 0xA0;
    dma->source = (u32)&fill;
    dma->destination = 0x07000000;
    dma->control = 0x85000100;
    dma->control;
    fill = 0;
    dma->source = (u32)&fill;
    dma->destination = 0x05000000;
    dma->control = 0x85000100;
    dma->control;
    REG_IE = 0x2001;
    REG_DISPSTAT = 8;
    REG_IME = 1;
}

void Mosaic_Initialize(void);

void sub_082156D8(u32);

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
    u32 fill, zero = 0;
    u8 byteZero, byteOne;
    struct VideoDmaRegisters *dma;
    u32 mask;
    struct RuntimeTask *a;
    struct RuntimeTask *b;
    fill = zero;
    dma = (struct VideoDmaRegisters *)REG_ADDR_DMA3SAD;
    dma->source = (u32)&fill;
    dma->destination = 0x06000000;
    dma->control = 0x85006000;
    (void)dma->control;
    fill = 0xa0;
    dma->source = (u32)&fill;
    dma->destination = 0x07000000;
    dma->control = 0x85000100;
    (void)dma->control;
    fill = zero;
    dma->source = (u32)&fill;
    dma->destination = 0x05000000;
    dma->control = 0x85000100;
    (void)dma->control;
    byteZero = 0;
    REG_IME = zero;
    (void)REG_IE;
    REG_IE = zero;
    gVideoInterruptsEnabled = 1;
    gVideoIrqPhase = zero;
    byteOne = 1;
    gVideoTransferControl = 1;
    mask = 0x2000;
    REG_IE = mask;
    REG_IE |= 0x1000;
    REG_IE |= 1;
    REG_IE |= 2;
    REG_DISPSTAT = 8;
    REG_DISPSTAT |= 0x10;
    REG_DISPSTAT |= 0x20;
    REG_DISPSTAT = (REG_DISPSTAT & 0xff) | 0x4000;
    REG_IME = 1;
    REG_KEYCNT = zero;
    gSpriteAttrFlags = zero;
    Mosaic_Initialize();
    sub_082156D8(0);
    Video_SetWindowBounds(0, 0, 0, 240, 160);
    Video_SetWindowBounds(1, 0, 0, 240, 160);
    Video_SetWindowLayerMasks(0x3f, 1, 0, 0);
    gQueuedObjSlotCount = zero;
    gSceneEntryActive = zero;
    Sprite_InitSystem();
    ObjShape_InitTables();
    Anim_ResetDefaultFlags();
    ScanlineEffect_Initialize();
    Text_LoadFontResource();
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    gSceneEffectPalettePhase = zero;
    gVBlankFrameCounter = zero;
    a = &gVideoUploadTask;
    Task_SetCallbacks(a, InterruptsAndVideo_OnVideoUploadTask, 0);
    b = &gGraphicsUpdateTask;
    Task_SetCallbacks(b, InterruptsAndVideo_OnGraphicsUpdateTask, 0);
    a->priority = byteZero;
    a->unk16 = byteOne;
    a->id = zero;
    b->priority = 13;
    b->unk16 = byteOne;
    b->id = zero;
    Task_LinkToPriorityList(a);
    Task_LinkToPriorityList(b);
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
    u16 i;
    REG_IME = 1;
    i = frames - 1;
    while (i != 0xFFFF) {
        InterruptsAndVideo_WaitForVBlank();
        i--;
    }
    REG_IME = savedInterruptMasterEnable;
}

// VBlank handler: sound, BG scroll/affine registers, HBlank DMA, link/clock ticks.

void m4aSoundVSync(void);

u32 WirelessController_IsCreated(void);

void RfuManager_CheckVBlankSync(void);

void SunClock_TickFrame(void);

#define InterruptsAndVideo_OnVBlank sub_08213FD8
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

#define InterruptsAndVideo_OnVCount sub_08214144
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

#define InterruptsAndVideo_RunIdleHook sub_082141B4
void InterruptsAndVideo_RunIdleHook(void)
{
}

/* InterruptsAndVideo_RunFrameHook: Unused empty hook near the frame callbacks; role tentative */

#define InterruptsAndVideo_RunFrameHook sub_082141B8
void InterruptsAndVideo_RunFrameHook(void)
{
}

void InterruptsAndVideo_OnUnusedInterrupt(u32 a)
{
}

void Bg_UpdateScrollShadow(void);

void ScanlineEffect_StageTransfer(void);

void Palette_UploadBuffers(void);

void sub_0821537C(void);

void Text_FlushTileUploads(void);

void sub_08215964(void);

void TileAnimation_Upload(void);

void sub_08219134(void);

u32 InterruptsAndVideo_OnVideoUploadTask(void)
{
    u32 *flags;
    vu16 *disp;
    Bg_UpdateScrollShadow();
    InterruptsAndVideo_WaitForVBlank();
    ScanlineEffect_StageTransfer();
    flags = &gVideoUploadFlags;
    if (*flags & 1) {
        struct VideoDmaRegisters *dma = (struct VideoDmaRegisters *)REG_ADDR_DMA3SAD;
        dma->source = (u32)gOamBuffer;
        dma->destination = 0x07000000;
        dma->control = 0x84000100;
        (void)dma->control;
        *flags &= ~1;
    }
    Palette_UploadBuffers();
    sub_0821537C();
    Text_FlushTileUploads();
    sub_08215964();
    TileAnimation_Upload();
    disp = &REG_DISPCNT;
    *disp &= 0xe0ff;
    if (!gSceneEntryActive) {
        u16 mask = gDispCntShadow;
        *disp |= mask | 0x1000;
    }
    sub_08219134();
    return gVBlankUploadPending = 0;
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
    if ((*(s32 *)&gGrayscaleControl)) {
        sub_08217C7C();
        sub_08217BBC();
        if ((*(s32 *)&gGrayscaleControl) > 0)
            (*(s32 *)&gGrayscaleControl)--;
        if (!(*(s32 *)&gGrayscaleControl))
            Palette_RefreshObjectBanks();
    } else {
        Palette_ApplyBackgroundFade();
        sub_08217430();
    }
    SpriteOam_ResetQueue(0);
    gSceneEffectPalettePhase++;
    return 0;
}
