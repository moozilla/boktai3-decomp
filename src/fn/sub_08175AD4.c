#include "global.h"
void TextWindow_SetBounds(u32, u32, u32, u32, u32);
void Hud_SetConfigThirdValueBounds(u8 *p)
{
    TextWindow_SetBounds(*(u32 *)(p + 0x48E0), 0x11, 7, 0xA, 1);
}
