#include "global.h"
#include "render.h"
#include "runtime.h"
// MATCHING_CONCESSION: register-pinning
// VBlank handler: sound, BG scroll/affine registers, HBlank DMA, link/clock ticks.
// mode, count and affine are pinned (register-pinning concession).
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
            register vu16 *affine asm("r2") = (vu16 *)0x04000020;
            u16 *matrix = gBgAffineMatrixBuffer;
            u32 *xy;
            *affine = matrix[0];
            affine++;
            *affine = matrix[1];
            affine++;
            *affine = matrix[2];
            affine++;
            *affine = matrix[3];
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
            register u32 count asm("r1");
            *(vu32 *)0x040000b8 = 0;
            *(vu32 *)0x040000b0 = (u32)gScanlineDmaSource;
            *(vu32 *)0x040000b4 = (u32)gScanlineDmaTarget;
            count = gScanlineDmaCount;
            if (mode != 1)
                *(vu32 *)0x040000b8 = 0xa6600000 | count;
            else
                *(vu32 *)0x040000b8 = 0xa2600000 | count;
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
