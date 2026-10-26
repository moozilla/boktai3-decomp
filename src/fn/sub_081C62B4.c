// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
struct TimeResult {
    u8 pad0[0x38];
    u8 digitGraphics[0xe0];
    struct Digit digits[6];
    u8 pad358[0x10];
    s32 elapsedFrames;
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

inline static u16 *GetY(struct DigitPos *pos)
{
    register u16 *ptr asm("r0") = &pos[0].y;
    asm("" : "+r"(ptr));
    return ptr;
}

void sub_081C62B4(u8 *p)
{
    struct DigitPos pos[6];
    s32 hours;
    s32 minutes;
    s32 seconds;
    s32 i;
    memcpy(pos, &gUnk_0824FA00, sizeof(pos));
    hours = Div(((struct TimeResult *)p)->elapsedFrames, 216000);
    minutes = Div(((struct TimeResult *)p)->elapsedFrames, 3600) - (hours * 60);
    seconds = (Div(((struct TimeResult *)p)->elapsedFrames, 60) - (hours * 3600)) - (minutes * 60);
    if (hours > 99) {
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[5],
                                  ((struct TimeResult *)p)->digitGraphics, 9);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[4],
                                  ((struct TimeResult *)p)->digitGraphics, 9);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[3],
                                  ((struct TimeResult *)p)->digitGraphics, 5);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[2],
                                  ((struct TimeResult *)p)->digitGraphics, 9);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[0],
                                  ((struct TimeResult *)p)->digitGraphics, 5);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[2],
                                  ((struct TimeResult *)p)->digitGraphics, 9);
    } else {
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[4],
                                  ((struct TimeResult *)p)->digitGraphics, Mod(hours, 10));
        hours = Div(hours, 10);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[5],
                                  ((struct TimeResult *)p)->digitGraphics, hours);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[2],
                                  ((struct TimeResult *)p)->digitGraphics, Mod(minutes, 10));
        minutes = Div(minutes, 10);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[3],
                                  ((struct TimeResult *)p)->digitGraphics, minutes);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[0],
                                  ((struct TimeResult *)p)->digitGraphics, Mod(seconds, 10));
        seconds = Div(seconds, 10);
        ChallengeResult_LoadDigit((u8 *)&((struct TimeResult *)p)->digits[1],
                                  ((struct TimeResult *)p)->digitGraphics, seconds);
    }
    {
        register u16 *sy asm("r6") = GetY(pos);
        u16 *dx = &((struct TimeResult *)p)->digits[0].x;
        u16 *sx = &pos[0].x;
        u8 *obj = p;
        for (i = 0; i < 6; i++) {
            *dx = (*sx) + 0x90;
            ((struct TimeResult *)obj)->digits[0].y = Arg1(0x38, sy);
            sy += 2;
            dx += 0x30;
            sx += 2;
            obj += 0x60;
        }
    }
}
