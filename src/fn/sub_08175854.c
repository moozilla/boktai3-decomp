#define SceneCatalog_SelectDescription sub_08175854
#include "global.h"
s32 MessageWindow_Clear(void);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_ShowMessage(s32);
void sub_08175854(s32 selection)
{
    if (selection < 0) {
        MessageWindow_Clear();
    } else {
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(selection);
    }
}
