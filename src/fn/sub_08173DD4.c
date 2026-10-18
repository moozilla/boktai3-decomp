#define SceneHud_InitEquipmentTransfer sub_08173DD4
#define SceneHud_SetWidgetPalette PauseMenu_SetSpritePalette
#define SceneHud_GetEquipmentSlotPosition sub_0816D920
#include "global.h"
#include "scene_hud_display.h"
s32 Inventory_GetEquipmentId(u32);
void sub_0816D920(void *,int);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *,u16);
void sub_08173DD4(struct SceneHudState *p,s32 index,s32 from,s32 to)
{
    struct SceneInputWidget *w=&p->motionWidgets[index];
    void *data;
    struct ContactPosition *target;
    w->flags &= ~1;
    data=p->motionResource;
    Anim_LoadKeyframe(w,data,(u16)(Inventory_GetEquipmentId(from)+0xf0),1);
    sub_0816D920(&w->position,from);
    target=&p->actionTargets[index];
    sub_0816D920(target,to);
    from+=10;
    PauseMenu_SetSpritePalette(w,p->categoryWidgets[from].paletteBank);
}
