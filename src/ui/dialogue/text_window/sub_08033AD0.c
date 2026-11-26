#include "global.h"
/* TextWindow_GetStringWidth: Measures an indexed substitution string including expanded numbers and wide characters */

u8 *TextWindow_Find(u32);
s32 TextState_MeasureStringLine(void *, u32);

s32 TextWindow_GetStringWidth(u32 id, u32 arg)
{
    u8 *w = TextWindow_Find(id);
    if (w == NULL)
        return -1;
    return TextState_MeasureStringLine(w + 0x60, arg);
}
