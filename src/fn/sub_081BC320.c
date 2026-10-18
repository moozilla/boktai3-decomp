#include "global.h"
void MessageWindow_SetPalette(u32);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void MessageWindow_SetDisplayMode(s32);
void DarkLoan_ShowPrompt(u8 *p)
{
    MessageWindow_SetPalette(0x2E9);
    MessageWindow_SetMessageTable(*(u32 *)(p + 0x8E8));
    MessageWindow_SetBounds(1, 0xf, 0x1c, 4);
    MessageWindow_SetDisplayMode(1);
}
