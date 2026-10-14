#include "global.h"
void DisplayAnimation_InitializeKeyframes(u8 *, s32, s32, u8 *);
void sub_0802CEF8(u8 *p, s32 a, s32 b)
{
    DisplayAnimation_InitializeKeyframes(p + 0x184, a, b, p + 0xC);
}
