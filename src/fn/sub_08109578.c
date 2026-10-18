#include "global.h"

void TextWindow_Clear(u32);

void LinkConnectionMenu_HideTextWindow(u8 *obj)
{
    TextWindow_Clear(*(u32 *)(obj + 0x30));
}
