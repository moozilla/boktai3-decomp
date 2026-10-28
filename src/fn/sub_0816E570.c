// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"
s32 Mod(s32, s32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void Sound_PlaySong(u32);
void SceneHud_GetActionIconPosition(void *, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 Selection_GetDisplayFrame(s32);
void SceneHud_BeginIconMotion(u8 *, u32, s32, u32, void *, void *, u32, u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void PauseMenu_OnCharacterSelectionUpdate(void);
struct IconMotionWidget {
    u32 flags;
    u8 pad[0x2E];
    u16 v;
    u8 tail[0x2C];
};
struct IconMotionView {
    u8 pad[0x1428];
    struct IconMotionWidget elems[266];
};
struct IconMotionPoint {
    u16 x, y;
    u32 unused;
};
struct IconMotionOffset {
    u32 x : 16, y : 16;
    u32 z;
};

#define NextRandom() (*index = (*index + 1) & 0x3ff, table[*index])

void sub_0816E570(u8 *s)
{
    struct IconMotionPoint pos;
    struct IconMotionOffset w;
    u8 *kind;
    u32 *flags;
    u16 speed;
    register const u16 *table asm("r6");
    u32 *index;

    kind = 0x438A + s;
    {
        u32 off = ((*kind) + 10) * sizeof(struct IconMotionWidget);
        u8 *base = s + 0x1428;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    SceneHud_GetActionIconPosition(&pos, *kind);
    pos.x += 8;
    pos.y += 8;
    table = gRandomTable;
    index = &gRandomIndex;
    w.x = -6 - Mod(NextRandom(), 3);
    w.y = -5 - Mod(NextRandom(), 5);
    speed = Mod(NextRandom(), 10) + 5;
    if (NextRandom() & 1)
        speed = -speed;
    SceneHud_BeginIconMotion(
        s, 0, Selection_GetDisplayFrame(Inventory_GetQuantityByStorage(s[0x4860], *kind)),
        ((struct IconMotionView *)s)->elems[*kind + 10].v, &pos, &w, 0, 0);
    *(u16 *)(s + 0x4404) = speed;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    Sound_PlaySong(0x27d);
    Hud_SetInteractionCallback(s, PauseMenu_OnCharacterSelectionUpdate, 1);
}
