#include "global.h"
void sub_080339B8(u32, u32, u32, u32, u32);
void Hud_SetConfigThirdValueBounds(u8 *p)
{
    sub_080339B8(*(u32 *)(p + 0x48E0), 0x11, 7, 0xA, 1);
}
