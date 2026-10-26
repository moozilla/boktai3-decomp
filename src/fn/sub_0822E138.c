#include "game_state.h"
void Inventory_SwapSlots(s32 first, s32 second)
{
    struct GameState *state = gGameState;
    s16 *a = &((struct GameState *)(first * 2 + (u32)state))->table160[0];
    s32 temp = *a;
    s16 *b = &((struct GameState *)(second * 2 + (u32)state))->table160[0];
    s32 i;
    *a = *b;
    *b = temp;
    for (i = 0; i <= 3; i++) {
        struct GameState *current = gGameState;
        s16 *slot = &((struct GameState *)(i * 2 + (u32)current))->party[0];
        if (*slot == first)
            *slot = second;
        else if (*slot == second)
            *slot = first;
    }
}
