#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
#include "runtime.h"
struct SelectionPanel {
    u8 unk0[0xA4E];
    u16 timer;
    u8 unkA50[0x48BC - 0xA50];
    u8 countdown;
    u8 unk48BD[0xB];
    u32 window;
};
void SceneCatalog_SelectDescription(u32);
void TextWindow_Clear(u32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_BeginSleepPrompt(void);
void Power_EnterSleep(void);
void SelectionPanel_WaitThenSleep(struct SelectionPanel *p)
{
    gSceneDialogActive = 1;
    if (++p->timer > 119) {
        SceneCatalog_SelectDescription(0x11);
        TextWindow_Clear(p->window);
        SceneHud_SetPairedIcons(p, 1, -1);
        Hud_SetInteractionCallback(p, PauseMenu_BeginSleepPrompt, 0);
        p->countdown = 30;
        Power_EnterSleep();
    }
}
