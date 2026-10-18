#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
struct SelectionPanel { u8 unk0[0x48C8]; u32 window; };
int SceneHud_UpdateBinaryChoice(void *);
void Sound_PlaySong(u32);
void SceneCatalog_SelectDescription(u32);
void TextWindow_Clear(u32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_BeginSleepPrompt(void);
void SelectionPanel_WaitThenSleep(void);
void SelectionPanel_ProcessChoice(struct SelectionPanel *p)
{
    int choice = SceneHud_UpdateBinaryChoice(p);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        SceneCatalog_SelectDescription(0x11);
        TextWindow_Clear(p->window);
        SceneHud_SetPairedIcons(p, 1, -1);
        Hud_SetInteractionCallback(p, PauseMenu_BeginSleepPrompt, 0);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        SceneCatalog_SelectDescription(0x13);
        Hud_SetInteractionCallback(p, SelectionPanel_WaitThenSleep, 1);
    }
}
