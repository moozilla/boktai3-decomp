#define PauseMenu_InitSleepTab sub_08176174
#define PauseMenu_BeginSleepPrompt sub_08176020
#define PauseMenu_SetCallback Hud_SetInteractionCallback
#include "global.h"
struct PauseMenuSleep {
    u8 unknown0[0x18];
    u32 resource;
    u8 unknown1C[0x10];
    u32 parameter;
    u8 unknown30[0x2C];
    const void *script;
    u8 unknown60[0x485C];
    u8 sleepState;
    u8 unknown48BD[7];
    u32 window0, window1;
};
void PauseMenu_CloseTextWindows(void *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void TextWindow_Clear(u32);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void Hud_SetupBackgroundLayer(void *, u32, s32, s32, u32);
void PauseMenu_HideAllSprites(void *);
void PauseMenu_LoadSpriteKeyframe(void *, s32, s32);
void MessageWindow_SetMessageTable(const void *);
void MessageWindow_SetPalette(s32);
void sub_08175854(u32);
void PauseMenu_SetPromptIconPositions(void *, s32, s32);
void sub_081660D0(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void sub_08176020(void);
void sub_08176174(struct PauseMenuSleep *s)
{
    PauseMenu_CloseTextWindows(s);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    TextWindow_Clear(s->window0);
    TextWindow_Clear(s->window1);
    MessageWindow_SetBounds(1, 0x10, 0x1D, 2);
    Hud_SetupBackgroundLayer(s, s->resource, 0, 11, s->parameter);
    PauseMenu_HideAllSprites(s);
    PauseMenu_LoadSpriteKeyframe(s, 0, 0x59);
    MessageWindow_SetMessageTable(s->script);
    MessageWindow_SetPalette(0xEFDA);
    sub_08175854(0x11);
    s->sleepState = 0;
    PauseMenu_SetPromptIconPositions(s, 0, -12);
    sub_081660D0(s, 1, -1);
    Hud_SetInteractionCallback(s, sub_08176020, 0);
}
