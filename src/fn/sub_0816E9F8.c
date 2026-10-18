#include "global.h"
s32 sub_0816E6F0(void *);
void SceneMenuPanel_ShowTimedResult(void);
void Hud_SetInteractionCallback(void *, void (*)(void), u32);
void PauseMenu_OnCharacterSelectionUpdate(u8 *s)
{
    s32 n = sub_0816E6F0(s);
    if (n >= 0) {
        s[0x4861] = n;
        Hud_SetInteractionCallback(s, SceneMenuPanel_ShowTimedResult, 1);
    }
}
