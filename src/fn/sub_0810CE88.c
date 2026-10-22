#include "global.h"
void TextWindow_Clear(u32);

void sub_0810CE88(u8 *p)
{
    TextWindow_Clear(*(u32 *)(p + 0x1408));
    TextWindow_Clear(*(u32 *)(p + 0x1400));
    TextWindow_Clear(*(u32 *)(p + 0x1404));
}
