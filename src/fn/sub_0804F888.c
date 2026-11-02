#include "game_state.h"
#include "repeated_effect.h"

void RepeatedEffect_TickLowerState(struct RepeatedEffect *p)
{
    if (gGameState->unk876 > 0) {
        u16 *c = (u16 *)&p->balance;
        u32 v = *c + 1;
        *c = v;
        if ((s32)(v << 16) > 0x012B0000)
            RepeatedEffect_SetState(p, 0, RepeatedEffect_TickLowerRecovery);
    }
}
