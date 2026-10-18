#include "global.h"
u8 *TextWindow_Find(void);
void TextWindow_OnIdle(void);
s32 TextWindow_StopPlayback(void)
{
    u32 *p = (u32 *)TextWindow_Find();
    s32 r;
    if (p) {
        p[0x5c / 4] = (u32)TextWindow_OnIdle;
        r = 0;
    } else
        r = -1;
    return r;
}
