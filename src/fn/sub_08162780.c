#include "global.h"
struct S {
    u8 f[0x77c];
    s32 x;
};
extern struct S *gGameState;
void PlayerBarrier_DecrementDuration(void)
{
    if (gGameState->x <= 1)
        gGameState->x = 0;
    else
        gGameState->x--;
}
