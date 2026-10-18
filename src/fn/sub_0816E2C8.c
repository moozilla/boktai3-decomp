#define MenuTransition_SetCallback Hud_SetInteractionCallback
#include "global.h"
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void SceneHud_GetActionIconPosition(void *, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 sub_0822D604(s32);
void SceneHud_BeginIconMotion(u8 *, u32, s32, u32, void *, void *, u32, u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void SceneMenuPanel_FinishMotionDelay(void);
struct MenuTransitionWidget {
    u32 flags;
    u8 pad[0x2E];
    u16 v;
    u8 tail[0x2C];
};
struct MenuTransitionView {
    u8 pad[0x1428];
    struct MenuTransitionWidget elems[266];
};
struct MenuTransitionCoordinates {
    u32 x : 16, y : 16;
    u32 z : 16, unused : 16;
};
void MenuTransition_BeginSelectedWidget(u8 *s)
{
    u32 v[2];
    struct MenuTransitionCoordinates w;
    const u8 *kind;
    u32 index;
    u32 later;
    u32 *flags;
    s32 result;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    kind = s + 0x438A;
    index = (*kind) + 10;
    {
        u32 off = index * sizeof(struct MenuTransitionWidget);
        u8 *base = s + 0x1428;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    w.x = 16;
    w.y = 48;
    w.z = 0;
    SceneHud_GetActionIconPosition(v, *kind);
    result = sub_0822D604(Inventory_GetQuantityByStorage(s[0x4860], *kind));
    later = (*kind) + 10;
    SceneHud_BeginIconMotion(s, 0, result, ((struct MenuTransitionView *)s)->elems[later].v, v, &w,
                             0x18, 0x10);
    Hud_SetInteractionCallback(s, SceneMenuPanel_FinishMotionDelay, 1);
}
