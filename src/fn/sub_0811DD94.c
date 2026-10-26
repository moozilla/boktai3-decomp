#include "global.h"
#include "game_state.h"
u32 StateFlags_GetCategoryBit(u32 category, u32 bit)
{
    u32 mask = 1 << bit;
    switch (category) {
        case 0:
            return gGameState->unk7B8[0] & mask;
        case 1:
            return gGameState->unk7B8[2] & mask;
        case 2:
            return gGameState->unk7B8[1] & mask;
        case 3:
            return gGameState->unk7B8[3] & mask;
        case 4:
            return gGameState->unk81C & mask;
        case 5:
            return gGameState->unk7F8 & mask;
    }
    return 0;
}
