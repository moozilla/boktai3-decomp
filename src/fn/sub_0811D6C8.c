// Tentative names: PalFade_Start sets up a timed fade between two 16-colour palette rows.
#include "global.h"
s32 Div(s32, s32);
void PalFade_Start(void *self, s32 p1, s32 p2, s32 p3, s32 p4, s32 duration, u32 flags, s32 mode);
extern u8 *gUnk_030042E4;
struct PalFade {
    u16 out[32];
    u16 timer, duration, p1, p2, p3, p4;
    s16 acc[3][32];
    s16 delta[3][32];
    u32 flags;
};
static inline s32 Trunc5(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
#define PAL(o) ((struct PalFade *)((u8 *)(o) + 0x348))
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining diff: spill-slot numbering and the order in which the per-row
// accumulator pointers are formed.
void PalFade_Start(void *self, s32 p1, s32 p2, s32 p3, s32 p4, s32 duration, u32 flags, s32 mode)
{
    struct PalFade *f = (struct PalFade *)(((u8 *)self) + 0x348);
    u16 *src2;
    s32 j;
    s32 k;
    u8 **slot;
    s16 *accrBase;
    s32 idx;
    *((u16 *)(((u8 *)self) + 0x38C)) = p1;
    *((u16 *)(((u8 *)self) + 0x38E)) = p2;
    *((u16 *)(((u8 *)self) + 0x390)) = p3;
    *((u16 *)(((u8 *)self) + 0x392)) = p4;
    for (j = 0; j < 2; j++) {
        u16 *src1;
        u16 *out;
        s16 *accr;
        s16 *accg;
        s16 *accb;
        accrBase = (s16 *)(((u8 *)self) + 0x394);
        if (j == 0) {
            src1 = (u16 *)(gUnk_030042E4 + (p1 * 32));
            src2 = (u16 *)(gUnk_030042E4 + (p3 * 32));
            slot = (u8 **)(((u8 *)self) + 0x80);
            if (mode == 1)
                src2 = f->out;
        } else {
            src1 = (u16 *)(gUnk_030042E4 + (p2 * 32));
            src2 = (u16 *)(gUnk_030042E4 + (p4 * 32));
            slot = (u8 **)(((u8 *)self) + 0x80);
            if (mode == 1)
                src2 = f->out + 16;
        }
        accr = accrBase + (j * 16);
        accg = f->acc[1] + (j * 16);
        accb = f->acc[2] + (j * 16);
        out = f->out + (j * 16);
        for (k = 0; k < 16; k++) {
            s32 n = (j * 16) + k;
            s32 c2 = src2[k];
            f->delta[0][n] = ((src1[k] & 0x1F) - (c2 & 0x1F)) << 5;
            f->delta[1][n] = (((src1[k] >> 5) & 0x1F) - ((c2 >> 5) & 0x1F)) << 5;
            f->delta[2][n] = (((src1[k] >> 10) & 0x1F) - ((c2 >> 10) & 0x1F)) << 5;
            f->delta[0][n] = Div(f->delta[0][n], duration);
            f->delta[1][n] = Div(f->delta[1][n], duration);
            f->delta[2][n] = Div(f->delta[2][n], duration);
            accr[k] = (c2 & 0x1F) << 5;
            accg[k] = ((c2 >> 5) & 0x1F) << 5;
            accb[k] = ((c2 >> 10) & 0x1F) << 5;
            idx = k;
            out[idx] = (Trunc5(accr[idx]) | (Trunc5(accg[idx]) << 5)) | (Trunc5(accb[idx]) << 10);
        }
    }

    {
        u8 *q = (*slot) + 0x180;
        for (j = 4; j >= 0; j--) {
            *((u8 **)(q + 0x48)) = (u8 *)f;
            q -= 0x60;
        }
    }
    f->timer = duration;
    f->duration = duration;
    f->flags = flags;
}

#else
INCLUDE_ASM("asm/nonmatching", PalFade_Start);
#endif
// PalFade_Update steps an active fade by one frame; returns 1 when the fade finishes or restarts.
u32 PalFade_Update(void *self)
{
    struct PalFade *f = PAL(self);
    s32 j, k;
    u16 *out;

    if (!(f->flags & 7))
        return 0;
    if (f->timer == 0) {
        if (f->flags & 2) {
            if ((f->flags & 8) && f->p4 == *(u16 *)((u8 *)self + 0x7E))
                f->flags = 1;
            PalFade_Start(self, f->p3, f->p4, f->p1, f->p2, f->duration, f->flags, 0);
        } else {
            if (!(f->flags & 4)) {
                u8 *q = *(u8 **)((u8 *)self + 0x80);
                u8 *v = q + 0x210;
                for (k = 4; k >= 0; k--) {
                    *(u8 **)(q + 0x48) = v;
                    q += 0x60;
                }
            }
            f->flags = 0;
        }
        return 1;
    }
    for (j = 0; j < 2; j++) {
        // Row base formed as offset + struct address, as in the ROM.
        out = (u16 *)(j * 32 + (u32)f);
        for (k = 0; k < 16; k++) {
            f->acc[0][j * 16 + k] += f->delta[0][j * 16 + k];
            f->acc[1][j * 16 + k] += f->delta[1][j * 16 + k];
            f->acc[2][j * 16 + k] += f->delta[2][j * 16 + k];
            out[k] = Trunc5(f->acc[0][j * 16 + k]) | Trunc5(f->acc[1][j * 16 + k]) << 5 |
                     Trunc5(f->acc[2][j * 16 + k]) << 10;
        }
    }
    if (--f->timer == 0)
        return 1;
    return 0;
}
