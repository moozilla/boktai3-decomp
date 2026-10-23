#include "global.h"
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
static inline s32 Interpolate(s32 start, s32 end, s32 tick, s32 duration)
{
    s32 delta;
    if (tick <= 0)
        return start;
    if (tick >= duration)
        return end;
    delta = __divsi3((s32)((u32)(end - start) << 8) * tick, duration);
    if (delta >= 0)
        delta >>= 8;
    else
        delta = -(-delta >> 8);
    return start + delta;
}
void StateActor_RunPaletteFlashSequence(u32 a, struct S *p, s32 a2)
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
            s32 base;
            s32 hi;
            s32 r2;
            if (a2 > 0x77)
                goto finalize;
            color = Interpolate(0x40, 0, a2, 0x78);
            DisplayAnimation_SetPalette(p->d4, color, color, 0);
            base = p->h56;
            hi = base + 0x40;
            r2 = Interpolate(base, hi, a2, 0x78);
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
