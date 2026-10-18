#include "global.h"

s32 TextWindow_Create(u32, u32, u32, u32);
void TextWindow_SetPlaybackRate(s32, u32);
void ModifierMenu_RefreshTextLabels(u8 *);

s32 ModifierMenu_CreateTextWindows(u8 *p)
{
    s32 *q = (s32 *)(p + 0xf0);
    s32 r;
    s32 i;
    s32 y;
    r = TextWindow_Create(1, 3, 10, 2);
    *q = r;
    if (r < 0)
        return -1;
    TextWindow_SetPlaybackRate(r, 5);
    q = (s32 *)(p + 0xf4);
    r = TextWindow_Create(1, 6, 0x1c, 6);
    *q = r;
    if (r < 0)
        return -1;
    TextWindow_SetPlaybackRate(r, 5);
    i = 2;
    y = 5;
    q = (s32 *)(p + 0xf8);
    do {
        r = TextWindow_Create(0xd, y, 0x10, 2);
        *q = r;
        if (r < 0)
            return -1;
        TextWindow_SetPlaybackRate(r, 1);
        y += 2;
        q++;
        i++;
    } while (i <= 7);
    ModifierMenu_RefreshTextLabels(p);
}
