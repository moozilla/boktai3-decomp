#define SceneHud_TestOwnerModeGate PauseMenu_IsEquipmentSelectionEnabled
#define SceneHud_LoadTertiaryIcon  PauseMenu_LoadStatusIconFrame
#define SceneHud_GetCarouselY      PauseMenu_GetEquipmentRowY
#include "global.h"
#include "scene_hud_display.h"
s32 PauseMenu_IsEquipmentSelectionEnabled(struct SceneHudState *);
void PauseMenu_LoadStatusIconFrame(struct SceneHudState *, struct SceneInputWidget *, s32);
s32 PauseMenu_GetEquipmentRowY(struct SceneHudState *, s32, s32, s32, s32);
void SceneHud_UpdateTertiaryWidgets(struct SceneHudState *p)
{
    s32 i;
    struct SceneInputWidget *w;
    s8 *selection;
    if (PauseMenu_IsEquipmentSelectionEnabled(p) == 1) {
        u32 one = 1;
        u32 *flags = &p->tertiaryWidgets[0].flags;
        for (i = 2; i >= 0; flags += 24, i--)
            *flags |= one;
    }
    i = 0;
    selection = &p->tertiarySelection;
    w = (struct SceneInputWidget *)((u8 *)p + 0xF70);
    for (; i <= 2; w++, i++) {
        struct SceneHudState *entry = (struct SceneHudState *)((u8 *)p + *selection * 4);
        PauseMenu_LoadStatusIconFrame(p, w, entry->tertiaryOptions[0].icon);
        w->position.y = PauseMenu_GetEquipmentRowY(p, i, *selection, 3, 0);
    }
}
