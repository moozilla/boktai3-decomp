#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.1%
// Remaining: register allocation only (affine/matrix in the BG2 matrix copy, the DMA control pointer).
struct Dma {
    vu32 src, dest, control;
};
extern u32 gUnk_03003A44, gUnk_0300426C, gUnk_03004268, gUnk_03003A48, gUnk_03003A4C, gUnk_03004280;
extern u16 gUnk_03004C20[], gUnk_03004BEC, gUnk_03004BF0[], gUnk_03004270;
extern u32 gUnk_03004C18[], gUnk_03004278, gUnk_03003A00, gUnk_03004260, gUnk_03004264;
extern s16 gUnk_03004274;
void m4aSoundVSync(void);
u32 WirelessController_IsCreated(void);
void RfuManager_CheckVBlankSync(void);
void SunClock_TickFrame(void);
void sub_08213FD8(void)
{
    m4aSoundVSync();
    gUnk_03003A44 = 1;
    if (gUnk_0300426C) {
        vu16 *regs;
        u16 *offset;
        s32 mode;
        gUnk_03004268 = gUnk_03003A48;
        gUnk_03003A4C = gUnk_03004280;
        regs = (vu16 *)0x04000010;
        offset = gUnk_03004C20;
        *regs = offset[0];
        regs++;
        *regs = offset[1];
        regs++;
        *regs = offset[2];
        regs++;
        *regs = offset[3];
        if (!gUnk_03004BEC) {
            regs++;
            *regs = offset[4];
            regs++;
            *regs = offset[5];
            regs++;
            *regs = offset[6];
            regs++;
            *regs = offset[7];
        } else {
            vu16 *affine = (vu16 *)0x04000020;
            u16 *matrix = gUnk_03004BF0;
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
                xy = gUnk_03004C18;
                *ref = xy[0];
                ref++;
                *ref = (xy[0] & 0x0fff0000) >> 16;
                ref++;
                *ref = xy[1];
                ref++;
                *ref = (xy[1] & 0x0fff0000) >> 16;
            }
        }
        mode = gUnk_03004274;
        if (mode) {
            vu32 *control = (vu32 *)0x040000b8;
            u32 count;
            *control = 0;
            *(vu32 *)0x040000b0 = gUnk_03004278;
            *(vu32 *)0x040000b4 = gUnk_03003A00;
            count = gUnk_03004270;
            if (mode != 1)
                *control = 0xa6600000 | count;
            else
                *control = 0xa2600000 | count;
        } else {
            struct Dma *dma = (struct Dma *)0x040000b0;
            dma->src = mode;
            dma->dest = mode;
            dma->control = mode;
            (void)dma->control;
        }
        gUnk_03004260 = 1;
        if (WirelessController_IsCreated())
            RfuManager_CheckVBlankSync();
        SunClock_TickFrame();
        gUnk_03004264++;
    }
    *(vu16 *)0x03007ff8 = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08213FD8);
#endif
