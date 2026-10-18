#include "global.h"
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void sub_0816EA98(void);
void Hud_SetInteractionCallback(void *, void (*)(void), u32);
void PauseMenu_BeginCharacterItemUse(u8 *s, u32 value)
{
    s[0x4861] = value;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    Hud_SetInteractionCallback(s, sub_0816EA98, 1);
}
