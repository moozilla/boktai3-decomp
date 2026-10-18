#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
int SceneHud_UpdateBinaryChoice(void *);
void Sound_PlaySong(u32);
void SceneCatalog_SelectDescription(u32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SelectionPanel_ConfirmTransition(void);
void SceneDialog_BeginTimedMessage(void);
void SelectionPanel_ProcessSecondChoice(void *p)
{
    int choice = SceneHud_UpdateBinaryChoice(p);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        SceneCatalog_SelectDescription(8);
        SceneHud_SetPairedIcons(p, 1, -1);
        Hud_SetInteractionCallback(p, SelectionPanel_ConfirmTransition, 0);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        Hud_SetInteractionCallback(p, SceneDialog_BeginTimedMessage, 1);
    }
}
