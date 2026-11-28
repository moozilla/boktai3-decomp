#include "global.h"
#include "link/lobby/record_menu.h"

void TextWindow_Clear(s32);

void LinkLobbyPanel_ClearTextWindows(struct RecordMenu *menu)
{
    TextWindow_Clear(menu->titleWindow);
    TextWindow_Clear(menu->nameWindows[0]);
    TextWindow_Clear(menu->nameWindows[1]);
}
