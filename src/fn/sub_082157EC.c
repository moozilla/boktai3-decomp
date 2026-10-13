#include "global.h"
#ifdef NONMATCHING
// fuzzy: 46.3%
// 13 differing instruction runs; size +4 bytes; first diff at +0x2 (ldr r0, [pc, #0x94] vs mov r7, sb)
extern u16 gUnk_03004BD8;
extern u32 gUnk_03004BC0[];
extern u16 gUnk_03004BE0[], gUnk_03004C08[];
extern const u32 gUnk_086140CC[];
void Bg_ClearTilemap(s32);
struct Dma {
    vu32 src, dst, ctl;
};
void sub_082157EC(void)
{
    u32 zero = 0;
    vu16 *bg, *blend;
    struct Dma *dma;
    u16 *a, *b;
    u32 *p;
    const u32 *src;
    u32 dst;
    s32 i;
    gUnk_03004BD8 = zero;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    bg = (vu16 *)0x04000008;
    bg[0] &= 0xffbf;
    bg[1] &= 0xffbf;
    bg[2] &= 0xffbf;
    *(vu16 *)0x0400000e &= 0xffbf;
    blend = (vu16 *)0x04000050;
    blend[0] = zero;
    blend[1] = zero;
    blend[2] = zero;
    i = 0;
    zero = 0;
    dma = (struct Dma *)0x040000d4;
    a = gUnk_03004BE0;
    p = gUnk_03004BC0;
    src = gUnk_086140CC;
    dst = 0x06000000;
    b = gUnk_03004C08;
    do {
        *p++ = zero;
        *a = zero;
        *b = zero;
        dma->src = (u32)src;
        dma->dst = dst;
        dma->ctl = 0x84000008;
        dma->ctl;
        a++;
        dst += 0x4000;
        b++;
        i++;
    } while (i <= 3);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_082157EC);
#endif
