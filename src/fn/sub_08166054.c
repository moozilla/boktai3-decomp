#include "global.h"
void Sound_PlaySong(s32);
void ScrollingDialogue_ClearScript(void);
void MessageWindow_Clear(void);
void MessageWindow_SetMessageTable(s32);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void MessageWindow_ShowMessage(s32);
void MessageWindow_SetDisplayMode(s32);
void PauseMenu_OpenPrompt(s32 a, s32 b, s32 c)
{
    if (c != 0)
        Sound_PlaySong(0x107);
    ScrollingDialogue_ClearScript();
    MessageWindow_Clear();
    MessageWindow_SetMessageTable(a);
    MessageWindow_SetBounds(0, 0x10, 0x1e, 2);
    MessageWindow_ShowMessage(b);
    MessageWindow_SetDisplayMode(1);
}
