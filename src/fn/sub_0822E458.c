#define Equipment_AssignUniqueSlot sub_0822E458
#include "game_state.h"
void sub_0822E458(s32 index, s32 id)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        struct GameState *state = gUnk_02000710;
        s16 *slot = &((struct GameState *)(i * 2 + (u32)state))->slots88[0];
        if (*slot == id)
            *slot = -1;
    }
    {
        struct GameState *state = gUnk_02000710;
        ((struct GameState *)(index * 2 + (u32)state))->slots88[0] = id;
    }
}
