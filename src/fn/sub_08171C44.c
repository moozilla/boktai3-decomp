#include "global.h"
void Sound_PlaySong(s32);
void PauseMenu_SmoothPoint(void *, void *, s32);
void PauseMenu_InterpolateActivePairPositions(void *, s32);
void Inventory_SwapSlots(s32, s32);
void PauseInventory_BeginConsumableSwapSelection(void *, s32);
void PauseInventory_UpdateConsumableSwapAnimation(u8 *p)
{
    u16 *timer = (u16 *)(p + 0xa4e);
    if (!*timer)
        Sound_PlaySong(0x38a);
    (*timer)++;
    if (*timer <= 11) {
        s32 n = 12 - *timer;
        PauseMenu_SmoothPoint(p + 0x2ac0, p + 0x442c, n);
        PauseMenu_SmoothPoint(p + 0x2b20, p + 0x4434, n);
        PauseMenu_InterpolateActivePairPositions(p, n);
    } else {
        u32 *q = (u32 *)(p + 0x2aa8);
        u32 v = *q, m = 1;
        *q = v | m;
        *(u32 *)(p + 0x2b08) |= m;
        *(u32 *)(p + 0x41e8) |= m;
        *(u32 *)(p + 0x4248) |= m;
        Inventory_SwapSlots(p[0x438a], p[0x4387]);
        PauseInventory_BeginConsumableSwapSelection(p, 0);
    }
}
