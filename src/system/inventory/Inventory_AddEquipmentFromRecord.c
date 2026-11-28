#include "global.h"
#include "system/game_state/game_state.h"
#include "player/player.h"
s32 Inventory_GetEquipmentId(s32);
void Inventory_SetEquipmentEntry(s32, const void *);
s32 Inventory_AddAssignedSlot(u32);
void Hud_RefreshInventoryDisplay(void);
void *Inventory_GetEquipmentEntry(s32);
void Player_BeginDescriptorAction(void *, void *);
s32 Inventory_AddEquipmentFromRecord(const void *record)
{
    s32 count, i, slot;
    const s32 empty = -1;

    for (i = 0; i < 16; i++) {
        if (Inventory_GetEquipmentId(i) == 0xFF)
            break;
    }
    if (i >= 16)
        return -1;
    Inventory_SetEquipmentEntry(i, record);
    slot = i;
    count = 0;
    for (i = 0; i < 8; i++) {
        if (gGameState->inventory[i] > empty)
            count++;
    }
    if (count <= 0) {
        gGameState->unk50 = 0;
        Inventory_AddAssignedSlot(slot);
        Hud_RefreshInventoryDisplay();
        if (gPlayers[0])
            Player_BeginDescriptorAction(gPlayers[0], Inventory_GetEquipmentEntry(slot));
    }
    return slot;
}
