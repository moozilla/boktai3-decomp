#include "global.h"

u8 *TextWindow_Find(u32);
s32 sub_08032D8C(void *, u32);

s32 sub_08033AD0(u32 id, u32 arg)
{
    u8 *w = TextWindow_Find(id);
    if (w == NULL)
        return -1;
    return sub_08032D8C(w + 0x60, arg);
}
