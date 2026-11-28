#include "global.h"
#include "ui/save/save_menu.h"
void TextWindow_Clear(u32);

void sub_0810FA0C(struct SaveMenu *menu)
{
    s32 *windows = &menu->nameWindow;
    s32 i;
    for (i = 0; i < 8; i++)
        TextWindow_Clear(windows[i]);
}
