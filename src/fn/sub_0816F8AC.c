#include "global.h"
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void SceneHud_GetActionIconPosition(void *, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 sub_0822D604(s32);
void SceneHud_BeginIconMotion(u8 *, u32, s32, u32, void *, void *, u32, u32);
void Sound_PlaySong(u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void MotionSequence_AdvanceAfterDelay(void);
struct E {
    u32 flags;
    u8 pad[0x2E];
    u16 v;
    u8 tail[0x2C];
};
struct S {
    u8 pad[0x1428];
    struct E elems[266];
};
void PauseMenu_BeginTypeFourteenItemUse(u8 *s)
{
    u32 v[2];
    u32 w[2];
    u8 *kind;
    u32 index;
    u32 later;
    u32 *flags;
    s32 result;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    do {
        ScrollingDialogue_RequestHide();
        kind = s + 0x438A;
        index = (*kind) + 10;
    } while (0);
    {
        u32 off = index * sizeof(struct E);
        u8 *base = s + 0x1428;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    SceneHud_GetActionIconPosition(v, *kind);
    SceneHud_GetActionIconPosition(w, s[0x4387]);
    result = sub_0822D604(Inventory_GetQuantityByStorage(s[0x4860], *kind));
    later = (*kind) + 10;
    SceneHud_BeginIconMotion(s, 0, result, ((struct S *)s)->elems[later].v, v, w, 0x18, 0x10);
    Sound_PlaySong(0x38A);
    Hud_SetInteractionCallback(s, MotionSequence_AdvanceAfterDelay, 1);
}
