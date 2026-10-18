#include "global.h"
#include "scene_hud_display.h"
s32 Inventory_GetConsumableId(u32);
void SceneHud_GetActionIconPosition(void *, int);
void PauseMenu_SetSpritePalette(void *, u16);
void SceneHud_PrepareActionTransferToSlot(struct SceneHudState *p, s32 index, s32 action, s32 target)
{
    struct SceneInputWidget *widget = &p->motionWidgets[index];
    void *resource;
    struct ContactPosition *position;
    widget->flags &= ~1;
    resource = p->motionResource;
    Anim_LoadKeyframe(widget, resource, (u16)(Inventory_GetConsumableId(action) + 0x121), 1);
    SceneHud_GetActionIconPosition(&widget->position, action);
    position = &p->actionTargets[index];
    SceneHud_GetActionIconPosition(position, target);
    PauseMenu_SetSpritePalette(widget, p->categoryWidgets[action + 10].paletteBank);
}
