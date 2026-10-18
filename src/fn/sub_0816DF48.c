#define InventoryGrid_GetItemId        Inventory_GetQuantityByStorage
#define InventoryGrid_SetPhaseCallback Hud_SetInteractionCallback
#include "global.h"
struct InventoryGridElement {
    u32 flags;
    u8 unknown04[0x2E];
    u16 value32;
    u8 unknown34[0x2C];
};
struct InventoryGrid {
    u8 unknown0000[0x1428];
    struct InventoryGridElement elements[266];
};
struct InventoryGridVector {
    s16 x, y, z, pad;
};
void SceneHud_GetActionIconPosition(void *, int);
s32 Inventory_GetQuantityByStorage(u32, u32);
u32 sub_0822D604(s32);
void SceneHud_BeginIconMotion(void *, u32, s32, u32, void *, void *, u32, u32);
void Sound_PlaySong(u32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void InventoryGrid_BeginNumberPhaseAlternate(void);
void InventoryGrid_BeginIconTransfer(u8 *s)
{
    struct InventoryGridVector from, to;
    u8 *kind;
    u32 index;
    u32 later;
    u32 *flags;
    s32 result;
    kind = s + 0x438A;
    index = *kind + 10;
    {
        u32 off = index * sizeof(struct InventoryGridElement);
        u8 *base = s + 0x1428;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    SceneHud_GetActionIconPosition(&from, s[0x4387]);
    to.x = 16;
    to.y = 48;
    to.z = 0;
    result = sub_0822D604(Inventory_GetQuantityByStorage(s[0x4860], *kind));
    later = *kind + 10;
    SceneHud_BeginIconMotion(s, 0, result, ((struct InventoryGrid *)s)->elements[later].value32,
                             &from, &to, 24, 16);
    Sound_PlaySong(0x38A);
    Hud_SetInteractionCallback(s, InventoryGrid_BeginNumberPhaseAlternate, 1);
}
