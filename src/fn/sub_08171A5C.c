#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_PrepareActionTransferToSlot sub_08171A5C
s32 Inventory_GetConsumableId(u32);
void sub_0816D920(void *, int);
void PauseMenu_SetSpritePalette(void *, u16);
void sub_08171A5C(struct SceneHudState *p, s32 index, s32 action, s32 target)
{
    struct SceneInputWidget *widget = &p->motionWidgets[index];
    void *resource;
    struct ContactPosition *position;
    widget->flags &= ~1;
    resource = p->motionResource;
    Anim_LoadKeyframe(widget, resource, (u16)(Inventory_GetConsumableId(action) + 0x121), 1);
    sub_0816D920(&widget->position, action);
    position = &p->actionTargets[index];
    sub_0816D920(position, target);
    PauseMenu_SetSpritePalette(widget, p->categoryWidgets[action + 10].paletteBank);
}
