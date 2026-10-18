#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
struct SelectionPanel { u8 unk0[0xA4E]; u16 timer; };
void SceneCatalog_SelectDescription(u32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SelectionPanel_ConfirmSettings(void);
void SelectionPanel_WaitConfirmDelay(struct SelectionPanel *p)
{
    if (++p->timer > 59) {
        SceneCatalog_SelectDescription(5);
        SceneHud_SetPairedIcons(p, 1, -1);
        Hud_SetInteractionCallback(p, SelectionPanel_ConfirmSettings, 0);
    }
}
