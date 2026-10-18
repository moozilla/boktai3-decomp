#include "global.h"
/* Actor_InitAnimationKeyframes: Initializes drawable keyframes using the actor position */
#define Actor_InitAnimationKeyframes sub_0802CEF8
void DisplayAnimation_InitializeKeyframes(u8 *, s32, s32, u8 *);
void sub_0802CEF8(u8 *p, s32 a, s32 b)
{
    DisplayAnimation_InitializeKeyframes(p + 0x184, a, b, p + 0xC);
}
