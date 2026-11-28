#include "global.h"
#include "system/game_state/game_state.h"
void Inventory_ClearEquipmentEntry(u32);
void Inventory_RemoveEquipmentEntry(s32 slot)
{
    s32 i;

    Inventory_ClearEquipmentEntry(slot);
    for (i = 0; i < 8; i++) {
        if (*(i + gGameState->inventory) == slot)
            *(i + gGameState->inventory) = -1;
    }
}
