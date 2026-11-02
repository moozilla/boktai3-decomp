#include "global.h"
#include "render.h"
#include "gba/gba.h"
#include "runtime.h"
// MATCHING_CONCESSION: register-pinning
// VBlank handler: sound, BG scroll/affine registers, HBlank DMA, link/clock ticks.
struct Dma {
    vu32 src, dest, control;
};
void m4aSoundVSync(void);
u32 WirelessController_IsCreated(void);
void RfuManager_CheckVBlankSync(void);
void SunClock_TickFrame(void);
void sub_08213FD8(void)
{
    m4aSoundVSync();
    gVBlankOccurred = 1;
    if (gVideoInterruptsEnabled) {
        vu16 *regs;
        u16 *offset;
        register s32 mode asm("r3");
        gDialogueScrollLatched = gDialogueScrollOffset;
        gDialogueModeLatched = gVBlankDialogueState;
        regs = (vu16 *)0x04000010;
        offset = gBgScrollShadow;
        *regs = offset[0];
        regs++;
        *regs = offset[1];
        regs++;
        *regs = offset[2];
        regs++;
        *regs = offset[3];
        if (!gBgAffineMode) {
            regs++;
            *regs = offset[4];
            regs++;
            *regs = offset[5];
            regs++;
            *regs = offset[6];
            regs++;
            *regs = offset[7];
        } else {
            u32 *xy;
            REG_BG2PA = gBgAffineMatrixBuffer[0];
            REG_BG2PB = gBgAffineMatrixBuffer[1];
            REG_BG2PC = gBgAffineMatrixBuffer[2];
            REG_BG2PD = gBgAffineMatrixBuffer[3];
            {
                vu16 *ref = (vu16 *)0x04000028;
                xy = gBgAffineRefBuffer;
                *ref = xy[0];
                ref++;
                *ref = (xy[0] & 0x0fff0000) >> 16;
                ref++;
                *ref = xy[1];
                ref++;
                *ref = (xy[1] & 0x0fff0000) >> 16;
            }
        }
        mode = gScanlineDmaMode;
        if (mode) {
            u32 count;
            REG_DMA0CNT = 0;
            REG_DMA0SAD = (u32)gScanlineDmaSource;
            REG_DMA0DAD = (u32)gScanlineDmaTarget;
            {
                u32 ctrl;
                count = gScanlineDmaCount;
                if (mode != 1) {
                    ctrl = 0xa6600000;
                    ctrl |= count;
                    REG_DMA0CNT = ctrl;
                } else {
                    ctrl = 0xa2600000;
                    ctrl |= count;
                    REG_DMA0CNT = ctrl;
                }
            }
        } else {
            struct Dma *dma = (struct Dma *)0x040000b0;
            dma->src = mode;
            dma->dest = mode;
            dma->control = mode;
            (void)dma->control;
        }
        gVBlankUploadPending = 1;
        if (WirelessController_IsCreated())
            RfuManager_CheckVBlankSync();
        SunClock_TickFrame();
        gVBlankFrameCounter++;
    }
    *(vu16 *)0x03007ff8 = 1;
}
