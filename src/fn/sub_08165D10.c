// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Open: the original keeps the frame/duration addresses in registers through the
// head, then recomputes and spills them for the loop; here they are spilled early.
struct PaletteBlendHud {
    u8 f0[0xA18];
    u16 buf[16];
    u16 from;
    u16 to;
    u8 frame;
    u8 duration;
    u8 active;
};
extern u16 *gUnk_030042E4;
s32 Div(s32, s32);
s32 SceneHud_TickPaletteBlend(struct PaletteBlendHud *p0)
{
    register struct PaletteBlendHud *p asm("sl") = p0;
    u16 *a;
    u16 *b;
    s32 i;
    s32 left;
    u8 *framep;
    u8 *durp;
    u32 k2 = 5;
    if (!p->active)
        return 1;
    a = (u16 *)(((u8 *)gUnk_030042E4) + (p->from << k2));
    framep = &p->frame;
    b = (u16 *)(((u8 *)gUnk_030042E4) + (p->to << k2));
    durp = &p->duration;
    if ((++(*framep)) >= (*durp)) {
        u16 *dst = p->buf;
        i = 15;
        do {
            *dst = *b;
            b++;
            dst++;
            i--;
        } while (i >= 0);
        p->active = 0;
        return 1;
    }
    left = (*durp) - (*framep);
    {
        for (i = 0; i <= 15; i++) {
            {
                u8 k1 = k2;
                if ((i != k1) && (i != 13)) {
                    u32 ca = *a;
                    u32 ar;
                    u32 ag;
                    u32 ab;
                    u32 cb;
                    u32 br;
                    u32 bg;
                    u32 bb;
                    u32 mask;
                    s32 r;
                    s32 g;
                    s32 bl;
                    ar = 31;
                    ar &= ca;
                    ag = (ca << 16) >> 21;
                    mask = 31;
                    ag &= mask;
                    ab = ((ca << 16) >> 26) & mask;
                    cb = *b;
                    br = 31;
                    asm("" : "+r"(br));
                    br &= cb;
                    bg = ((cb << 16) >> 21) & mask;
                    bb = ((cb << 16) >> 26) & mask;
                    r = Div((ar * left) + (br * (*framep)), *durp);
                    g = Div((ag * left) + (bg * (*framep)), *durp);
                    bl = Div((ab * left) + (bb * (*framep)), *durp);
                    p->buf[i] = ((bl << 10) | (g << k2)) | r;
                }
            }
            a++;
            b++;
        }
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneHud_TickPaletteBlend);
#endif
