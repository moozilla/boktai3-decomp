#include "global.h"

void TextWindow_Clear(u32);

void LinkLobbyPanel_ClearTextWindows(u8 *p)
{
    TextWindow_Clear(*(u32 *)(p + 0x14F8));
    TextWindow_Clear(*(u32 *)(p + 0x14F0));
    TextWindow_Clear(*(u32 *)(p + 0x14F4));
}
