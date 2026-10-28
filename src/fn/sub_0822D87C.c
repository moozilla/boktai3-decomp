#include "global.h"
#include "game_state.h"
#include "player.h"
s32 Inventory_GetEquipmentId(s32);
void Inventory_SetEquipmentEntry(s32, const void *);
s32 Inventory_AddAssignedSlot(u32);
void Hud_RefreshInventoryDisplay(void);
void *Inventory_GetEquipmentEntry(s32);
void Player_BeginDescriptorAction(void *, void *);
s32 Inventory_AddEquipmentFromRecord(const void *record)
{
    s32 count, i;
    s16 *q;
    void *p;
    u8 **gp;
    s32 idx;
    s32 m1;
    for (i = 0; i <= 15; i++) {
        if (Inventory_GetEquipmentId(i) == 0xFF)
            break;
    }
    if (i > 15)
        return -1;
    Inventory_SetEquipmentEntry(i, record);
    idx = i;
    count = 0;
    {
        u8 **t = &gGameState;
        q = (s16 *)*t;
        m1 = -1;
        gp = t;
    }
    q = (s16 *)((u8 *)q + 0x68);
    i = 7;
    do {
        s32 z = 0;
        if (*(s16 *)((u8 *)q + z) > m1)
            count++;
        q++;
        i--;
    } while (i >= 0);
    if (count <= 0) {
        *(u16 *)(*gp + 0x50) = 0;
        Inventory_AddAssignedSlot(idx);
        Hud_RefreshInventoryDisplay();
        p = (*(void **)gPlayers);
        if (p)
            Player_BeginDescriptorAction(p, Inventory_GetEquipmentEntry(idx));
    }
    return idx;
}
