#define SceneHud_SetWidgetPalette PauseMenu_SetSpritePalette
#include "global.h"
#include "scene_hud_display.h"
s32 Inventory_GetEquipmentId(u32);
void SceneHud_GetActionIconPosition(void *, int);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u16);
void SceneHud_InitEquipmentTransfer(struct SceneHudState *p, s32 index, s32 from, s32 to)
{
    struct SceneInputWidget *w = &p->motionWidgets[index];
    void *data;
    struct ContactPosition *target;
    w->flags &= ~1;
    data = p->motionResource;
    Anim_LoadKeyframe(w, data, (u16)(Inventory_GetEquipmentId(from) + 0xf0), 1);
    SceneHud_GetActionIconPosition(&w->position, from);
    target = &p->actionTargets[index];
    SceneHud_GetActionIconPosition(target, to);
    from += 10;
    PauseMenu_SetSpritePalette(w, p->categoryWidgets[from].paletteBank);
}
