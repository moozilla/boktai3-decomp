#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.0%
// fix.py best (sweep after the inlined-insn fix)
// Loop pointer registers differ: ROM keeps the y-source pointer in r6 (copied from r0) and the counter in r5.
struct DigitPos {
    u16 x, y;
};
struct DigitPosTable {
    struct DigitPos e[6];
};
struct Digit {
    u8 pad0[0x20];
    u16 x, y;
    u8 pad24[0x3c];
};
extern const struct DigitPosTable gUnk_0824FA00;
void ChallengeResult_LoadDigit(u8 *, void *, s32);
s32 Div(s32, s32);
void memcpy(void *, const void *, u32);
s32 Mod(s32, s32);

inline static u32 Arg1(u32 k_, u16 *sy)
{
    return (*sy) + k_;
}

void sub_081C62B4(u8 *p)
{
    struct DigitPos pos[6];
    s32 hours;
    s32 minutes;
    s32 seconds;
    s32 i;
    memcpy(pos, &gUnk_0824FA00, sizeof(pos));
    hours = Div(*((s32 *)(p + 0x368)), 216000);
    minutes = Div(*((s32 *)(p + 0x368)), 3600) - (hours * 60);
    seconds = (Div(*((s32 *)(p + 0x368)), 60) - (hours * 3600)) - (minutes * 60);
    if (hours > 99) {
        ChallengeResult_LoadDigit(p + 0x2f8, p + 0x38, 9);
        ChallengeResult_LoadDigit(p + 0x298, p + 0x38, 9);
        ChallengeResult_LoadDigit(p + 0x238, p + 0x38, 5);
        ChallengeResult_LoadDigit(p + 0x1d8, p + 0x38, 9);
        ChallengeResult_LoadDigit(p + 0x118, p + 0x38, 5);
        ChallengeResult_LoadDigit(p + 0x1d8, p + 0x38, 9);
    } else {
        ChallengeResult_LoadDigit(p + 0x298, p + 0x38, Mod(hours, 10));
        hours = Div(hours, 10);
        ChallengeResult_LoadDigit(p + 0x2f8, p + 0x38, hours);
        ChallengeResult_LoadDigit(p + 0x1d8, p + 0x38, Mod(minutes, 10));
        minutes = Div(minutes, 10);
        ChallengeResult_LoadDigit(p + 0x238, p + 0x38, minutes);
        ChallengeResult_LoadDigit(p + 0x118, p + 0x38, Mod(seconds, 10));
        seconds = Div(seconds, 10);
        ChallengeResult_LoadDigit(p + 0x178, p + 0x38, seconds);
    }
    {
        u16 *sy = &pos[0].y;
        u16 *dx = (u16 *)(p + 0x138);
        u16 *sx = &pos[0].x;
        u8 *obj = p;
        for (i = 0; i < 6; i++) {
            *dx = (*sx) + 0x90;
            *((u16 *)(obj + 0x13a)) = Arg1(0x38, sy);
            sy += 2;
            dx += 0x30;
            sx += 2;
            obj += 0x60;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081C62B4);
#endif
