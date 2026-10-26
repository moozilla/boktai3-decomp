// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#include "engine.h"
s32 Mod(s32, s32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void SceneHud_GetActionIconPosition(void *, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 Selection_GetDisplayFrame(s32);
void SceneHud_BeginIconMotion(u8 *, u32, s32, u32, void *, void *, u32, u32);
void Sound_PlaySong(u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void SceneHud_AnimateActionResult(void);
static inline s32 Panel_RandomPercent(void)
{
    u32 next = gRandomIndex + 1;
    u16 mask;
    asm("" ::: "r1");
    mask = 0x3FF;
    gRandomIndex = next & mask;
    return Mod(*(u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR), 100);
}
void ScenePanel_BeginRandomTransition(u8 *s)
{
    u32 v[2];
    u32 w[2];
    u8 *kind;
    u32 index;
    u32 later;
    u32 *flags;
    s32 result;
    do {
        kind = s + 0x438A;
        index = (*kind) + 10;
    } while (0);
    {
        u32 off = index * sizeof(struct SceneInputWidget);
        u8 *base = s + 0x1428;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    SceneHud_GetActionIconPosition(v, *kind);
    SceneHud_GetActionIconPosition(w, s[0x4387]);
    result = Selection_GetDisplayFrame(Inventory_GetQuantityByStorage(s[0x4860], *kind));
    later = (*kind) + 10;
    SceneHud_BeginIconMotion(s, 0, result,
                             ((struct SceneHudState *)s)->categoryWidgets[later].paletteBank, v, w,
                             0x18, 0x10);
    Sound_PlaySong(0x38A);
    if (((struct SceneHudState *)s)->actionOwner->actionMode == 6 || Panel_RandomPercent() < 20)
        s[0x4861] = 1;
    else
        s[0x4861] = 0;
    Hud_SetInteractionCallback(s, SceneHud_AnimateActionResult, 1);
}
