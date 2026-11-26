#include "global.h"
void TextWindow_Clear(u32);

void sub_0810BF60(u8 *p)
{
    TextWindow_Clear(*(u32 *)(p + 0xf68));
    TextWindow_Clear(*(u32 *)(p + 0xf60));
    TextWindow_Clear(*(u32 *)(p + 0xf64));
}
