#include "global.h"
#include "system/game_state/game_state.h"
void MenuPanel_SwapConsumableSlots(u32 source, u32 destination)
{
    s32 old = *(source + gGameState->table160);
    s32 i;
    *(source + gGameState->table160) = *(destination + gGameState->table160);
    *(destination + gGameState->table160) = old;
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
