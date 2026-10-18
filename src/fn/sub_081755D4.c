#include "global.h"
struct SceneCatalog {
    u8 unknown0[0x5C];
    const u8 *descriptionResource;
    u8 unknown60[0x4864];
    s32 textWindow;
};
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void SceneCatalog_ResetTextWindow(struct SceneCatalog *s)
{
    s32 *window = &s->textWindow;
    TextWindow_SetScript(*window, s->descriptionResource);
    TextWindow_Refresh(*window, 0);
    TextWindow_StartPlayback(*window);
}
