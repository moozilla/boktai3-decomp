#include "system/game_state/game_state.h"
void Inventory_AssignUniqueSecondarySlot(s32 id, s32 index)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (*(i + &gGameState->inventory[8]) == id)
            *(i + &gGameState->inventory[8]) = -1;
    }
    *(index + &gGameState->inventory[8]) = id;
}
