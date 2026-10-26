#include "global.h"
#define gGameStateBytes gGameState
extern u8 *gGameState;
s32 GameState_GetPackedTripletValue(s32 a, s32 b)
{
    s32 i = a * 3 + b;
    s32 h = i / 2;
    s32 odd = i & 1;
    u8 *q = gGameState + 0x658 + h;
    if (odd) {
        s32 value = *q;
        if (value >= 0)
            return value >> 4;
        return -((-value) >> 4);
    }
    return *q & 15;
}
