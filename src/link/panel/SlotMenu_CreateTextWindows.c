#include "global.h"
#include "link/panel/link_panel.h"
s32 TextWindow_Create(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void SlotMenu_CreateTextWindows(struct LinkPanel *panel)
{
    panel->mainWindow = TextWindow_Create(1, 3, 28, 2);
    panel->firstWindow = TextWindow_Create(1, 6, 10, 2);
    panel->secondWindow = TextWindow_Create(1, 6, 10, 2);
    TextWindow_SetScript(panel->mainWindow, panel->selectionScript);
    TextWindow_Refresh(panel->mainWindow, 0);
    TextWindow_StartPlayback(panel->mainWindow);
}
