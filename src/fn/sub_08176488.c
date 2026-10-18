#define SceneDialog_SetPhase Hud_SetInteractionCallback
#include "global.h"
struct SceneDialog {
    u8 pad0[0x5c];
    const u8 *script;
    u8 pad60[0xA4E - 0x60];
    u16 timer;
    u8 padA50[0x48C8 - 0xA50];
    s32 window;
};
void Sound_PlaySong(u32);
void TextWindow_Clear(u32);
void SceneCatalog_SelectDescription(u32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SelectionPanel_ConfirmTransition(void);
void SceneDialog_FinishFailedResult(struct SceneDialog *p)
{
    if (!p->timer) {
        Sound_PlaySong(0x192);
        TextWindow_Clear(p->window);
        SceneCatalog_SelectDescription(15);
    }
    if (++p->timer > 119) {
        SceneCatalog_SelectDescription(8);
        SceneHud_SetPairedIcons(p, 1, -1);
        Hud_SetInteractionCallback(p, SelectionPanel_ConfirmTransition, 0);
    }
}
