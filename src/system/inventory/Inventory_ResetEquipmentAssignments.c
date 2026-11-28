#include "system/game_state/game_state.h"

void Inventory_ResetEquipmentAssignments(void)
{
    s32 i;

    for (i = 0; i < 8; i++)
        *(i + gGameState->inventory) = -1;
    gGameState->unk50 = 8;
}
