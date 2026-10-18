#include "global.h"
void TextWindow_Clear(u32);
void PauseMenu_CloseTextWindows(u8 *p)
{
    u32 *q = (u32 *)(p + 0x48C4);
    s32 i = 7;
    do {
        TextWindow_Clear(*q++);
        i--;
    } while (i >= 0);
}
