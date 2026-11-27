#include "global.h"
void TextWindow_Clear(u32);
void StageResults_DestroyTextWindows(u8 *p)
{
    u32 *q = (u32 *)(p + 0x11CC);
    s32 i = 2;
    do {
        TextWindow_Clear(*q++);
    } while (--i >= 0);
}
