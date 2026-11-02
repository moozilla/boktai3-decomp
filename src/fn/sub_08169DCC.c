#include "global.h"
#include "ram.h"
#include "scene_hud_display.h"
#include "player.h"

void Hud_LoadConditionalPaletteRows(void *);
void sub_08168850(void *);
void SceneHud_UpdateSecondaryWidgets(void *);
void SceneHud_RebuildPrimaryOptions(void *);
void PauseMenu_RefreshEquipmentSlotIcons(void *);
void sub_08168214(void *);
u32 WirelessGame_IsItemToggleEnabled(void);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void sub_08164480(void *, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
void Hud_SetUpdateCallback(void *, void (*)(void));
void SceneHud_UpdateOptionControls(void);
void SceneHud_EnterOptionControls(void)
{
    struct SceneHudState *p = (void *)gFieldHud;
    if (p) {
        Hud_LoadConditionalPaletteRows(p);
        sub_08168850(p);
        SceneHud_UpdateSecondaryWidgets(p);
        SceneHud_RebuildPrimaryOptions(p);
        PauseMenu_RefreshEquipmentSlotIcons(p);
        sub_08168214(p);
        p->auxiliaryToggle = 0;
        if (WirelessGame_IsItemToggleEnabled()) {
            struct SceneHudControlAnimation *box = &p->controlAnimation;
            box->flags &= ~1;
            Anim_LoadKeyframe(box, p->controlAnimationResource, 0x1C, 1);
        }
        p->actionOwner->currentStatB = p->actionOwner->maximumStatB;
        sub_08164480(p, 0, 4, 2, p->actionOwner->currentStatB, p->actionOwner->maximumStatB, 1);
        Sound_PlaySong(0x10C);
        p->pulseFrame = 0;
        Hud_SetUpdateCallback(p, SceneHud_UpdateOptionControls);
    }
}
