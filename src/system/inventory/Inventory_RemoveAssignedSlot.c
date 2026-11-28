#include "system/game_state/game_state.h"

u32 Inventory_IsEquipmentAssigned(s32);

s32 Inventory_RemoveAssignedSlot(s32 slot)
{
    s32 list[8];
    s32 n = 0;
    s32 i;
    for (i = 0; i < 16; i++) {
        if (i == slot) {
            if (n < 8) {
                if (n == (s16)gGameState->unk50)
                    gGameState->unk50 = 0;
                else if (n < (s16)gGameState->unk50)
                    gGameState->unk50--;
            }
        } else if (Inventory_IsEquipmentAssigned(i)) {
            list[n] = i;
            n++;
        }
    }
    if (n == 0)
        return 0;
    for (; n < 8; n++)
        list[n] = -1;
    for (i = 0; i < 8; i++)
        *(i + gGameState->inventory) = list[i];
    return 1;
}
