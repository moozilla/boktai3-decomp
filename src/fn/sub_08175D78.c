#define PauseMenu_SetCallback Hud_SetInteractionCallback
#include "global.h"
#include "input.h"
struct PauseMenuConfig {
    u8 unknown0[0x64];
    u8 animationData[0x159C];
    u8 animation[4];
};
void Sound_PlaySong(u32);
s32 Anim_Step(void *, void *);
void SceneCatalog_SaveOptions(struct PauseMenuConfig *);
void SelectionPanel_CaptureSettings(struct PauseMenuConfig *);
void sub_0817597C(struct PauseMenuConfig *, u32);
void SceneCatalog_SelectDescription(u32);
void Hud_SetInteractionCallback(struct PauseMenuConfig *, void (*)(void), u32);
void SceneHud_SetPairedIcons(struct PauseMenuConfig *, u32, s32);
void MenuSelection_RefreshAllOptionText(struct PauseMenuConfig *);
s32 MenuSelection_ProcessOptionInput(struct PauseMenuConfig *);
void SelectionPanel_WaitConfirmDelay(void);
void SelectionPanel_ConfirmSettings(void);
void PauseMenu_UpdateConfigChoice(struct PauseMenuConfig *s)
{
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        SceneCatalog_SaveOptions(s);
        sub_0817597C(s, 0);
        SceneCatalog_SelectDescription(7);
        Hud_SetInteractionCallback(s, SelectionPanel_WaitConfirmDelay, 1);
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        SelectionPanel_CaptureSettings(s);
        sub_0817597C(s, 0);
        SceneCatalog_SelectDescription(5);
        SceneHud_SetPairedIcons(s, 1, -1);
        MenuSelection_RefreshAllOptionText(s);
        Hud_SetInteractionCallback(s, SelectionPanel_ConfirmSettings, 0);
    } else {
        if (MenuSelection_ProcessOptionInput(s))
            sub_0817597C(s, 1);
        Anim_Step(s->animation, s->animationData);
    }
}
