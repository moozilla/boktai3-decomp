#include "global.h"
#include "game_state.h"
void StateFlags_SetCategoryBit(u32 category, u32 bit)
{
    u32 mask = 1 << bit;
    switch (category) {
        case 0:
            gGameState->unk7B8[0] |= mask;
            break;
        case 1:
            gGameState->unk7B8[2] |= mask;
            break;
        case 2:
            gGameState->unk7B8[1] |= mask;
            break;
        case 3:
            gGameState->unk7B8[3] |= mask;
            break;
        case 4:
            gGameState->unk81C |= mask;
            break;
        case 5:
            gGameState->unk7F8 |= mask;
            break;
    }
}
