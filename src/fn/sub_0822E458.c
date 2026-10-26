#include "game_state.h"
void Equipment_AssignUniqueSlot(s32 index, s32 id)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        struct GameState *state = gGameState;
        s16 *slot = &((struct GameState *)(i * 2 + (u32)state))->slots88[0];
        if (*slot == id)
            *slot = -1;
    }
    {
        struct GameState *state = gGameState;
        ((struct GameState *)(index * 2 + (u32)state))->slots88[0] = id;
    }
}
