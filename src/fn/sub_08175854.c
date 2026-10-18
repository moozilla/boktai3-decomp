#include "global.h"
s32 MessageWindow_Clear(void);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_ShowMessage(s32);
void SceneCatalog_SelectDescription(s32 selection)
{
    if (selection < 0) {
        MessageWindow_Clear();
    } else {
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(selection);
    }
}
