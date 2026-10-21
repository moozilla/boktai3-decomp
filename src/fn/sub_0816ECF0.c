// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.3%
// Remaining: destination-address reuse, source loads and zero scheduling.
#include "scene_hud_display.h"
void SceneHud_GetActionIconPosition(void *, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 Selection_GetDisplayFrame(s32);
void SceneHud_BeginIconMotion(u8 *, u32, s32, u32, void *, void *, u32, u32);
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void SceneMenuPanel_FinishResultMotion(void);
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
struct RatioHud {
    u8 pad[0xa40];
    u8 *owner;
    u8 pad2[0x4868 - 0xa44];
    u16 prevA, nextA, prevB, nextB;
};
#define OWNER(p) (((struct RatioHud *)(p))->owner)
// confidence=low: mode zero exchanges the proportions of two current vitals.
void PauseMenu_BeginVitalRatioExchange(u8 *s, u32 mode)
{
    u32 from[2];
    u32 to[2];
    u8 *kind;
    u32 index;
    u32 later;
    u32 *flags;
    s32 result;
    u16 *target;
    u16 v;
    kind = s + 0x438a;
    index = *kind + 10;
    {
        u32 off = index * sizeof(struct SceneInputWidget);
        u8 *base = s + 0x1428;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    SceneHud_GetActionIconPosition(from, *kind);
    SceneHud_GetActionIconPosition(to, s[0x4387]);
    result = Selection_GetDisplayFrame(Inventory_GetQuantityByStorage(s[0x4860], *kind));
    later = *kind + 10;
    SceneHud_BeginIconMotion(s, 0, result,
                             ((struct SceneHudState *)s)->categoryWidgets[later].paletteBank, from,
                             to, 24, 16);
    if (mode == 0) {
        s[0x487f] = 1;
        {
            register u32 off asm("r4") = 0x424;
            u16 w;
            asm("" : "+r"(off));
            w = H(OWNER(s), off);
            ((struct RatioHud *)s)->prevA = w;
            off += 4;
            v = H(OWNER(s), off);
        }
        target = &((struct RatioHud *)s)->prevB;
        *target = v;
        ((struct RatioHud *)s)->nextA = Div(H(OWNER(s), 0x426) * *target, H(OWNER(s), 0x42a));
        if (!((struct RatioHud *)s)->nextA && *target)
            ((struct RatioHud *)s)->nextA = 1;
        ((struct RatioHud *)s)->nextB = Div(
            H(OWNER(s), 0x42a) * *(target = &((struct RatioHud *)s)->prevA), H(OWNER(s), 0x426));
        if (!((struct RatioHud *)s)->nextB && *target)
            ((struct RatioHud *)s)->nextB = 1;
        s[0x4861] = 0;
    } else {
        s[0x487f] = 2;
        ((struct RatioHud *)s)->prevA = H(OWNER(s), 0x424);
        ((struct RatioHud *)s)->prevB = H(OWNER(s), 0x428);
        ((struct RatioHud *)s)->nextA = 0;
        ((struct RatioHud *)s)->nextB = H(OWNER(s), 0x42a);
        s[0x4861] = 1;
    }
    Sound_PlaySong(0x38a);
    Hud_SetInteractionCallback(s, SceneMenuPanel_FinishResultMotion, 1);
}

#else
INCLUDE_ASM("asm/nonmatching", PauseMenu_BeginVitalRatioExchange);
#endif
