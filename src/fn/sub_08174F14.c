#include "global.h"
struct SceneCatalog {
    u8 unknown0[0x5C];
    const void *descriptionResource;
};
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_Clear(void);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
void SceneCatalog_SetDescription(struct SceneCatalog *s, s32 selection)
{
    if (selection < 0) {
        MessageWindow_Clear();
    } else {
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(s->descriptionResource);
        MessageWindow_SetBounds(0, 18, 30, 2);
        MessageWindow_ShowMessage(selection);
    }
}
