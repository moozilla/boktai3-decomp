#include "system/game_state/game_state.h"
s32 Inventory_GetItemPackedState(s32);
void Inventory_SetItemPackedState(s32, u32);
void sub_0822D080(s32 a, s32 b)
{
    s32 value = *(a + gGameState->table0A0);

    *(a + gGameState->table0A0) = *(b + gGameState->table0A0);
    *(b + gGameState->table0A0) = value;
    value = Inventory_GetItemPackedState(a);
    Inventory_SetItemPackedState(a, Inventory_GetItemPackedState(b));
    Inventory_SetItemPackedState(b, value);
}
