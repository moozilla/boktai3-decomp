#define SceneHud_FindCategoryAction Inventory_GetQuantityByStorage
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_GetActionIconPosition(void *, s32);
s32 SceneHud_FindCategoryAction(u32, u32);
u32 sub_0822D604(s32);
void SceneHud_BeginIconMotion(void *, u32, s32, u32, void *, void *, u32, u32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void InventoryGrid_BeginNumberPhase(void);
void SceneHud_BeginSelectedActionTransfer(struct SceneHudState *s)
{
    struct ContactPosition from;
    struct SceneInputPackedPosition to;
    u8 *selection = &s->actionSelection;
    u32 index = *selection + 10;
    u32 offset = index * 0x60;
    u8 *base = (u8 *)s + 0x1428;
    u32 *flags = (u32 *)(base + offset);
    s32 keyframe;
    u32 later;
    *flags |= 1;
    SceneHud_GetActionIconPosition(&from, s->actionCursor);
    to.x = 16;
    to.y = 48;
    to.z = 0;
    keyframe = sub_0822D604(SceneHud_FindCategoryAction(s->actionCategory, *selection));
    later = *selection + 10;
    SceneHud_BeginIconMotion(s, 0, keyframe, s->categoryWidgets[later].paletteBank, &from, &to, 24,
                             16);
    Sound_PlaySong(0x38a);
    Hud_SetInteractionCallback(s, InventoryGrid_BeginNumberPhase, 1);
}
