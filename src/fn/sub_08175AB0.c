#include "global.h"
void TextWindow_SetBounds(u32, u32, u32, u32, u32);
void Hud_SetConfigSecondValueBounds(u8 *p)
{
    TextWindow_SetBounds(*(u32 *)(p + 0x48D8), 0x11, 5, 0xA, 1);
}
