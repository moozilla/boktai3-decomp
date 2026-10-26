#include "global.h"
#include "game_state.h"
void MenuPanel_SwapConsumableSlots(u32 source, u32 destination)
{
    struct GameState *game = gGameState;
    struct GameState *a = (struct GameState *)((source << 1) + (u32)game);
    s32 old = a->table160[0];
    struct GameState *b = (struct GameState *)((destination << 1) + (u32)game);
    s32 i;
    a->table160[0] = b->table160[0];
    b->table160[0] = old;
    for (i = 0; i <= 3; i++) {
        u32 address = i;
        struct GameState *current = gGameState;
        struct GameState *indexed;
        s16 *slot;
        address <<= 1;
        address += (u32)current;
        indexed = (struct GameState *)address;
        slot = &indexed->party[0];
        if (*slot == source) {
            if (destination <= 15)
                *slot = destination;
            else
                *slot = -1;
        } else if (*slot == destination) {
            if (source <= 15)
                *slot = source;
            else
                *slot = -1;
        }
    }
}
