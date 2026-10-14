#include "global.h"

u8 *TextWindow_Find(u32);
void sub_08032CC8(void *, u32, u32);

s32 sub_08033A10(u32 id, u32 a, u32 b)
{
    u8 *w = TextWindow_Find(id);
    if (w == NULL)
        return -1;
    sub_08032CC8(w + 0x60, a, b);
    return 0;
}
