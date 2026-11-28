#include "system/game_state/game_state.h"

/* Store the low 15 bits of item state `index`, keeping bit 15. */
void Inventory_SetItemStateValue(s32 index, u32 value)
{
    *(index + gGameState->table100) = (value & 0x7FFF) | (*(index + gGameState->table100) & 0x8000);
}
