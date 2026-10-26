#include "global.h"
#include "game_state.h"
s32 Inventory_GetItemPackedState(s32);
void Inventory_SetItemPackedState(s32, u32);
void sub_0822D080(s32 a, s32 b)
{
    s32 new_var;
    u8 *ctx = gGameState;
    s16 *first = (s16 *)((a * 2 + (u32)ctx) + 0xa0);
    s32 v = *first;
    *first = *(s16 *)(b * 2 + ctx + 0xa0);
    *(s16 *)(b * 2 + ctx + 0xa0) = v;
    new_var = a;
    v = Inventory_GetItemPackedState(a);
    Inventory_SetItemPackedState(new_var, Inventory_GetItemPackedState(b));
    Inventory_SetItemPackedState(b, v);
}
