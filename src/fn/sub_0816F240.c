#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#ifdef NONMATCHING
// fuzzy: 96.6%
// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order
#define ScenePanel_BeginRandomTransition sub_0816F240
extern u32 gUnk_03005308;
s32 Mod(s32, s32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void SceneHud_GetActionIconPosition(void *, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 sub_0822D604(s32);
void SceneHud_BeginIconMotion(u8 *, u32, s32, u32, void *, void *, u32, u32);
void Sound_PlaySong(u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void sub_0816F340(void);
static inline s32 Panel_RandomPercent(void)
{
    u32 next = gUnk_03005308 + 1;
    u16 mask = 0x3FF;
    gUnk_03005308 = next & mask;
    return Mod(*(u16 *)((gUnk_03005308 << 1) + RANDOM_TABLE_ADDR), 100);
}
void sub_0816F240(u8 *s)
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
    result = sub_0822D604(Inventory_GetQuantityByStorage(s[0x4860], *kind));
    later = (*kind) + 10;
    SceneHud_BeginIconMotion(s, 0, result, ((struct SceneHudState *)s)->categoryWidgets[later].paletteBank, v, w, 0x18, 0x10);
    Sound_PlaySong(0x38A);
    if (((struct SceneHudState *)s)->actionOwner->actionMode == 6 ||
        Panel_RandomPercent() < 20)
        s[0x4861] = 1;
    else
        s[0x4861] = 0;
    Hud_SetInteractionCallback(s, sub_0816F340, 1);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0816F240);
#endif
