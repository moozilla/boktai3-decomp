#include "modifier_list.h"

void TextWindow_Destroy(s32);

void ModifierMenu_DestroyTextWindows(struct ModifierMenu *menu)
{
    u8 *bytes = (u8 *)menu;
    u32 firstWindow = (u8 *)&menu->mainWindow - bytes;
    s32 i;
    for (i = 0; i < 8; i++) {
        s32 *window = (void *)&bytes[firstWindow + i * sizeof(*window)];
        if (*window >= 0) {
            TextWindow_Destroy(*window);
            *window = -1;
        }
    }
}
