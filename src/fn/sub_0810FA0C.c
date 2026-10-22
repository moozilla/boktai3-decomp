#include "global.h"
void TextWindow_Clear(u32);

void sub_0810FA0C(u8 *p)
{
    u32 *windows = (u32 *)(p + 0x1004);
    s32 i;
    for (i = 7; i >= 0; i--)
        TextWindow_Clear(*windows++);
}
