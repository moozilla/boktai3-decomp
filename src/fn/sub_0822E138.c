#define Inventory_SwapSlots sub_0822E138
#include "game_state.h"
void sub_0822E138(s32 first, s32 second)
{
    struct GameState *state = gUnk_02000710;
    s16 *a = &((struct GameState *)(first * 2 + (u32)state))->table160[0];
    s32 temp = *a;
    s16 *b = &((struct GameState *)(second * 2 + (u32)state))->table160[0];
    s32 i;
    *a = *b;
    *b = temp;
    for (i = 0; i <= 3; i++) {
        struct GameState *current = gUnk_02000710;
        s16 *slot = &((struct GameState *)(i * 2 + (u32)current))->party[0];
        if (*slot == first)
            *slot = second;
        else if (*slot == second)
            *slot = first;
    }
}
