#include "system/game_state/game_state.h"

/* Set the override bit of item state `index`. */
void Inventory_SetItemOverrideFlag(s32 index)
{
    *(index + gGameState->table100) |= 0x8000;
}
