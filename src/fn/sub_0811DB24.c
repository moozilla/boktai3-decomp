#include "global.h"
struct G {
    u8 f[0x634];
    u32 v;
};
extern struct G *gGameState;
u32 GameState_SetChallengeCompletionFlag(u32 i)
{
    u32 r;
    if (i <= 11) {
        gGameState->v |= 1 << i;
        r = 1;
    } else
        r = 0;
    return r;
}
