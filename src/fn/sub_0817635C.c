#define SceneDialog_SetPhase Hud_SetInteractionCallback
#include "global.h"
#include "runtime.h"
struct SceneDialog {
    u8 pad0[0x5c];
    const u8 *script;
    u8 pad60[0xA4E - 0x60];
    u16 timer;
    u8 padA50[0x48C8 - 0xA50];
    s32 window;
};
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(int);
void SceneCatalog_SelectDescription(u32);
void SceneDialog_SelectResultPhase(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SceneDialog_BeginTimedMessage(struct SceneDialog *p)
{
    gSceneDialogActive = 1;
    if (!p->timer) {
        TextWindow_SetBounds(p->window, 2, 6, 26, 6);
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 13);
        TextWindow_StartPlayback(p->window);
        SceneCatalog_SelectDescription(12);
    }
    if (++p->timer > 59)
        Hud_SetInteractionCallback(p, SceneDialog_SelectResultPhase, 1);
}
