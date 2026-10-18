#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_PrepareActionTransfer sub_0817160C
s32 Inventory_GetConsumableId(u32);
s32 Inventory_GetConsumableCategory(u32);
void sub_0816D920(void *, int);
void PauseMenu_SetSpritePalette(void *, u16);
void sub_0817160C(struct SceneHudState *p, s32 index, s32 action)
{
    struct SceneInputWidget *widget = &p->motionWidgets[index];
    void *resource;
    s32 column;
    widget->flags &= ~1;
    resource = p->motionResource;
    Anim_LoadKeyframe(widget, resource, (u16)(Inventory_GetConsumableId(action) + 0x121), 1);
    sub_0816D920(&widget->position, action);
    column = Inventory_GetConsumableCategory(action);
    p->actionTargets[index].x = column * 24 + 8;
    p->actionTargets[index].y = 104;
    PauseMenu_SetSpritePalette(widget, p->categoryWidgets[action + 10].paletteBank);
}
