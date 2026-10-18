#define SceneHud_BeginRandomActionReturn sub_0816DC78
#define SceneHud_GetActionIconPosition sub_0816D920
#define SceneItem_SelectRandomSpecialItem Inventory_RollIcePopStick
#define SceneItem_SetSlotItem Inventory_SetItemId
#define SceneHud_BeginIconMotion sub_08165E78
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_GetActionIconPosition(void *, s32);
u32 SceneItem_SelectRandomSpecialItem(void);
void SceneItem_SetSlotItem(s32, s32);
u32 sub_0822D604(s32);
void SceneHud_BeginIconMotion(void *, u32, s32, u32, void *, void *, u32, u32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void sub_0816DD70(void);
void sub_0816DC78(struct SceneHudState *s)
{
    struct SceneInputPackedPosition from;
    struct ContactPosition to;
    u8 *selection = &s->actionSelection;
    u32 index = *selection + 10;
    u32 offset = index * 0x60;
    u8 *base = (u8 *)s + 0x1428;
    u32 *flags = (u32 *)(base + offset);
    u32 item;
    u32 later;
    s32 keyframe;
    *flags |= 1;
    *(u32 *)((u8 *)s + 0x2aa8) &= ~1;
    from.x = 16;
    from.y = 48;
    from.z = 0;
    SceneHud_GetActionIconPosition(&to, s->actionCursor);
    item = SceneItem_SelectRandomSpecialItem();
    SceneItem_SetSlotItem(*selection, SceneItem_SelectRandomSpecialItem());
    keyframe = sub_0822D604(item);
    later = *selection + 10;
    SceneHud_BeginIconMotion(s, 0, keyframe, s->categoryWidgets[later].paletteBank, &from, &to, 24, 16);
    Sound_PlaySong(0x38a);
    Hud_SetInteractionCallback(s, sub_0816DD70, 1);
}
