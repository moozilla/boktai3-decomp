#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.9%
// foundry draft (worker best 81.5%); what differs is mostly register allocation and instruction order
#define StateActor_RunPaletteFlashSequence sub_081C85F4
struct S {
    u8 f0[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 f3[4];
    u16 h0e;
    u32 w10;
    u8 f5[4];
    u8 b18;
    u8 f6[0x56 - 0x19];
    s16 h56;
    u8 f8[0x6a - 0x58];
    s16 h6a;
    u8 f9[0xd4 - 0x6c];
    u8 d4[1];
};
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
void DisplayAnimation_SetFlags(void *, u32);
void DisplayAnimation_ClearFlags(void *, u32);
s32 DisplayAnimation_SetPalette(void *, u32, u32, u32);
void Sound_PlaySong(u32);
s32 __divsi3(s32, s32);
void sub_081FED40(s32);
void sub_081C85F4(u32 a, struct S *p, s32 a2)
{
    s32 *base20 = (s32 *)((u8 *)p + 0x20);
    {
        u16 mask = 0x400;
        p->h0e |= mask;
    }
    if (p->b9 != 0) {
        u32 animId;
        p->b9 = 0;
        p->b7 = 0;
        DisplayAnimation_SetFlags(p->d4, 2);
        switch (p->b1) {
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
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                animId = 0;
                break;
            case 27:
                animId = 0x10;
                break;
            default:
                animId = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animId, 1, 1, 4);
        p->b18 = 0;
        p->w10 = 0;
        a2 = 0;
        Sound_PlaySong(0x577);
    }
    switch (p->b18) {
        case 0: {
            s32 color;
            s16 base;
            s32 hi;
            s32 r2;
            if (a2 > 0x77)
                goto finalize;
            if (a2 > 0)
                goto colorUpper;
            color = 0x40;
            goto colorDone;
        colorUpper:
            if (a2 < 0x78)
                goto colorCompute;
            color = 0;
            goto colorDone;
        colorCompute: {
            s32 t = a2 * (s32)0xFFFFC000;
            t = __divsi3(t, 0x78);
            if (t < 0)
                t = -(-t >> 8);
            else
                t = t >> 8;
            color = t + 0x40;
        }
        colorDone:
            DisplayAnimation_SetPalette(p->d4, color, color, 0);
            base = p->h56;
            hi = base + 0x40;
            if (a2 > 0)
                goto posUpper;
            r2 = base;
            goto posDone;
        posUpper:
            if (a2 < 0x78)
                goto posCompute;
            r2 = hi;
            goto posDone;
        posCompute: {
            s32 d = hi - base;
            d = d << 8;
            d = __divsi3(d * a2, 0x78);
            if (d < 0)
                d = -(-d >> 8);
            else
                d = d >> 8;
            r2 = base + d;
        }
        posDone:
            p->h6a = r2;
            break;
        finalize:
            DisplayAnimation_SetPalette(p->d4, 0x40, 0x40, 0);
            DisplayAnimation_ClearFlags(p->d4, 2);
            sub_081FED40(base20[1]);
            p->b18++;
            {
                u16 mask = 0x100;
                p->h0e |= mask;
            }
            break;
        }
        case 1: {
            u16 mask = 0x100;
            p->h0e |= mask;
            p->b7 = 1;
            break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C85F4);
#endif
