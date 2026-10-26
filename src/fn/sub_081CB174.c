#include "global.h"
#include "render.h"

#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
extern const s16 gUnk_086149C4[];

static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

void StateActor_UpdateHoverBob(u32 a, void *p, u32 flags)
{
    u32 r;
    if (B(9) != 0) {
        B(9) = 0;
        B(7) = 0;
        switch (B(1)) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                r = 0;
                break;
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                r = 0;
                break;
            case 29:
                r = 0xb;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
        CpuSet(gPaletteBankData + 0x5C0, (u8 *)p + 0x1CC, 0x04000008);
    }
    {
        u16 m = 0x400;
        H(0xE) = m | H(0xE);
    }
    {
        s32 val = (s32)gUnk_086149C4[(flags << 2) & 0xFF] << 4;
        s32 dx = shift12(val);
        u16 *q = (u16 *)*(void **)((u8 *)p + 0x160) + 1;
        u32 t = *q;
        t = t + 0x80;
        t = t + dx;
        *q = t;
    }
    if (flags & 0x10) {
        u16 m = 0x100;
        H(0xE) = m | H(0xE);
    }
}
