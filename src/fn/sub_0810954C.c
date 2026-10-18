#include "global.h"

u32 TextWindow_Create(u32, u32, u32, u32);
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_StartPlayback(u32);

void LinkConnectionMenu_CreateTextWindow(u8 *p)
{
    u32 h;

    h = TextWindow_Create(1, 6, 0x1C, 6);
    *(u32 *)(p + 0x30) = h;
    TextWindow_SetScript(h, *(u32 *)(p + 0x24));
    TextWindow_Refresh(*(u32 *)(p + 0x30), 0);
    TextWindow_StartPlayback(*(u32 *)(p + 0x30));
}
