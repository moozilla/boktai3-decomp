#define SceneHud_TestOwnerModeGate PauseMenu_IsEquipmentSelectionEnabled
#define SceneHud_LoadTertiaryIcon  PauseMenu_LoadStatusIconFrame
#define SceneHud_GetCarouselY      PauseMenu_GetEquipmentRowY
#include "global.h"
#include "ui/pause/scene_hud_display.h"
s32 PauseMenu_IsEquipmentSelectionEnabled(struct SceneHudState *);
void PauseMenu_LoadStatusIconFrame(struct SceneHudState *, struct SceneInputWidget *, s32);
s32 PauseMenu_GetEquipmentRowY(struct SceneHudState *, s32, s32, s32, s32);
void SceneHud_UpdateTertiaryWidgets(struct SceneHudState *p)
{
    s32 i;
    if (PauseMenu_IsEquipmentSelectionEnabled(p) == 1) {
        for (i = 0; i < 3; i++)
            p->tertiaryWidgets[i].flags |= 1;
    }
    for (i = 0; i <= 2; i++) {
        PauseMenu_LoadStatusIconFrame(p, &p->tertiaryWidgets[i],
                                      p->tertiaryOptions[p->tertiarySelection].icon);
        p->tertiaryWidgets[i].position.y =
            PauseMenu_GetEquipmentRowY(p, i, p->tertiarySelection, 3, 0);
    }
}
