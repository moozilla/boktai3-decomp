#include "game_state.h"
void Inventory_AssignUniqueSecondarySlot(s32 id, s32 index)
{
    s32 i;
    for (i = 0; i <= 7; i++) {
        struct GameState *state = gGameState;
        s16 *slot = &((struct GameState *)(i * 2 + (u32)state))->inventory[8];
        if (*slot == id)
            *slot = -1;
    }
    {
        struct GameState *state = gGameState;
        ((struct GameState *)(index * 2 + (u32)state))->inventory[8] = id;
    }
}
