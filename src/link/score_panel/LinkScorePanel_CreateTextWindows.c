#include "global.h"
#include "ui/save/save_menu.h"
u32 TextWindow_Create(u32, u32, u32, u32);

void LinkScorePanel_CreateTextWindows(struct SaveMenu *menu)
{
    menu->nameWindow = TextWindow_Create(1, 3, 10, 2);
    menu->windows[0] = TextWindow_Create(14, 12, 15, 2);
    menu->windows[1] = TextWindow_Create(14, 15, 15, 2);
    menu->windows[2] = TextWindow_Create(14, 17, 15, 2);
    menu->windows[3] = TextWindow_Create(14, 3, 15, 2);
    menu->windows[4] = TextWindow_Create(18, 6, 2, 2);
    menu->windows[5] = TextWindow_Create(26, 6, 2, 2);
    menu->windows[6] = TextWindow_Create(18, 8, 8, 2);
}
