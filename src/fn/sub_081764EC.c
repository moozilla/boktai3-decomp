#include "global.h"
#define PauseMenu_EnterPrompt sub_081764EC
void PauseMenu_CloseTextWindows(u8 *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void TextWindow_Clear(u32);
void MessageWindow_SetBounds(u32, u32, u32, u32);
void Hud_SetupBackgroundLayer(u8 *, u32, u32, u32, u32);
void PauseMenu_HideAllSprites(u8 *);
void PauseMenu_LoadSpriteKeyframe(u8 *, s32, u32);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_SetPalette(u32);
void SceneCatalog_SelectDescription(u32);
void PauseMenu_SetPromptIconPositions(u8 *, u32, s32);
void SceneHud_SetPairedIcons(u8 *, u32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SelectionPanel_ConfirmTransition(void);
void PauseMenu_EnterPrompt(u8 *s)
{
    PauseMenu_CloseTextWindows(s);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    TextWindow_Clear(*(u32 *)(s + 0x48C4));
    TextWindow_Clear(*(u32 *)(s + 0x48C8));
    MessageWindow_SetBounds(1, 0x10, 0x1D, 2);
    Hud_SetupBackgroundLayer(s, *(u32 *)(s + 0x18), 0, 0xB, *(u32 *)(s + 0x2C));
    PauseMenu_HideAllSprites(s);
    PauseMenu_LoadSpriteKeyframe(s, 0, 0x5A);
    MessageWindow_SetMessageTable(*(u32 *)(s + 0x5C));
    MessageWindow_SetPalette(0xEFDA);
    SceneCatalog_SelectDescription(8);
    PauseMenu_SetPromptIconPositions(s, 0, -12);
    SceneHud_SetPairedIcons(s, 1, -1);
    Hud_SetInteractionCallback(s, SelectionPanel_ConfirmTransition, 0);
}
