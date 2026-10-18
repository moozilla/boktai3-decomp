#define SceneHud_FindCategoryAction Inventory_GetQuantityByStorage
#define SceneHud_SetIconPalette     PauseMenu_SetSpritePalette
#include "global.h"
#include "scene_hud_display.h"
s32 SceneHud_FindCategoryAction(u32, u32);
u32 sub_0822D604(s32);
void SceneHud_GetActionIconPosition(void *, s32);
void SceneHud_SetIconPalette(struct SceneInputWidget *, u16);
void SceneHud_BeginActionIconMotion(struct SceneHudState *s, s32 index, s32 source, s32 target)
{
    struct SceneInputWidget *w = &s->motionWidgets[index];
    void *resource;
    w->flags &= ~1;
    resource = s->motionResource;
    Anim_LoadKeyframe(w, resource,
                      (u16)sub_0822D604(SceneHud_FindCategoryAction(s->actionCategory, source)), 1);
    SceneHud_GetActionIconPosition(&w->position, source);
    SceneHud_GetActionIconPosition(&s->actionTargets[index], target);
    source += 10;
    s = (struct SceneHudState *)((u8 *)s + source * 0x60);
    s = (struct SceneHudState *)((u8 *)s + 0x145a);
    SceneHud_SetIconPalette(w, *(u16 *)s);
}
