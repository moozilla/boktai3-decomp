#include "global.h"
u8 *TextWindow_Find(u32);
void TextWindow_OnPlayback(void);
s32 TextWindow_StartPlayback(u32 id)
{
    u32 *p = (u32 *)TextWindow_Find(id);
    s32 r;
    if (p) {
        p[0x5c / 4] = (u32)TextWindow_OnPlayback;
        r = 0;
    } else
        r = -1;
    return r;
}
