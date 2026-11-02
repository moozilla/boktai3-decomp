#include "global.h"
#include "render.h"
#include "gba/gba.h"
#include "runtime.h"
// VBlank handler: sound, BG scroll/affine registers, HBlank DMA, link/clock ticks.
void m4aSoundVSync(void);
u32 WirelessController_IsCreated(void);
void RfuManager_CheckVBlankSync(void);
void SunClock_TickFrame(void);
void sub_08213FD8(void)
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
