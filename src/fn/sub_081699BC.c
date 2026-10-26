#define SceneHud_BeginControlPhase       Hud_SetUpdateCallback
#define SceneHud_SetPulseHighlight       Hud_DrawHealthMeter
#define ScenePanel_ToggleAuxiliaryOption WirelessGame_ToggleItemMode
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_CyclePrimarySecondaryOptions(struct SceneHudState *);
void Hud_DrawHealthMeter(struct SceneHudState *, u32, u32, u32, u32);
void ResultPanel_DrawCappedCounters(struct SceneHudState *, u32, u32, u32);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(struct SceneHudState *));
void SelectionPanel_RefreshPrimaryIfReady(struct SceneHudState *);
int WirelessGame_ToggleItemMode(void);
void SceneHud_UpdateOptionControls(struct SceneHudState *p)
{
    if (!(gEnginePauseFlags & 1) && p->controlMode <= 2) {
        SceneHud_CyclePrimarySecondaryOptions(p);
        if (p->pulseFrame <= 63)
            Hud_DrawHealthMeter(p, 0, 4, 1, 1);
        else
            ResultPanel_DrawCappedCounters(p, 0, 3, 1);
        if (++p->pulseFrame > 127)
            p->pulseFrame = 0;
        if (gUnk_03005260[1] & 8)
            Hud_SetUpdateCallback(p, SelectionPanel_RefreshPrimaryIfReady);
        else if (gUnk_03005260[1] & 4) {
            if (p->auxiliaryToggle = WirelessGame_ToggleItemMode())
                Anim_LoadKeyframe(&p->controlAnimation, p->controlAnimationResource, 0x1B, 1);
            else
                Anim_LoadKeyframe(&p->controlAnimation, p->controlAnimationResource, 0x1C, 1);
        }
    }
}
