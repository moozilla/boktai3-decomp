#include "global.h"
#include "engine/engine.h"
#include "link/lobby/record_menu.h"

s32 TextWindow_Create(s32, s32, s32, s32);
void TextWindow_StartPlayback(s32);

void LinkLobbyPanel_CreateTextWindows(struct RecordMenu *menu)
{
    menu->titleWindow = TextWindow_Create(1, 3, 28, 2);
    menu->nameWindows[0] = TextWindow_Create(1, 6, 10, 2);
    menu->nameWindows[1] = TextWindow_Create(19, 6, 10, 2);
    TextWindow_SetScript(menu->titleWindow, menu->choiceScript);
    TextWindow_Refresh(menu->titleWindow, menu->selection);
    TextWindow_StartPlayback(menu->titleWindow);
}
