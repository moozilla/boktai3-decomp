#define PauseMenu_SetCallback Hud_SetInteractionCallback
#include "global.h"
#include "input.h"
struct PauseMenuSleep {
    u8 unknown0[0x5C];
    const u8 *script;
    u8 unknown60[0x4868];
    s32 promptWindow;
};
void Sound_PlaySong(u32);
void PauseMenu_SetConfirmationIndicator(struct PauseMenuSleep *, s32);
void SceneCatalog_SelectDescription(u32);
void PauseMenu_HidePromptIcons(struct PauseMenuSleep *);
void Hud_SetInteractionCallback(struct PauseMenuSleep *, void (*)(void), s32);
void SelectionPanel_ProcessChoice(void);
void MenuDisplay_SetPaletteOverrides(struct PauseMenuSleep *, s32, s32, s32, s32);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void PauseMenu_BeginSleepPrompt(struct PauseMenuSleep *s)
{
    s32 *window;
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        PauseMenu_SetConfirmationIndicator(s, 0);
        SceneCatalog_SelectDescription(0x12);
        PauseMenu_HidePromptIcons(s);
        Hud_SetInteractionCallback(s, SelectionPanel_ProcessChoice, 1);
        MenuDisplay_SetPaletteOverrides(s, 0, 0, 1, 0);
        window = &s->promptWindow;
        TextWindow_SetBounds(*window, 4, 6, 0x16, 6);
        TextWindow_SetScript(*window, s->script);
        TextWindow_Refresh(*window, 0x10);
        TextWindow_StartPlayback(*window);
    }
}
