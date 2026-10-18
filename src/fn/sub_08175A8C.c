#include "global.h"
void sub_080339B8(u32, u32, u32, u32, u32);
void Hud_SetConfigFirstValueBounds(u8 *p)
{
    sub_080339B8(*(u32 *)(p + 0x48D0), 0x11, 3, 0xf, 2);
}
