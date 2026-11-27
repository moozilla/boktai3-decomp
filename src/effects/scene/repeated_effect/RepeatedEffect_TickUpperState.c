#include "effects/scene/repeated_effect.h"
#include "system/game_state/game_state_flags.h"

void RepeatedEffect_TickUpperState(struct RepeatedEffect *p)
{
    if (GameState_TestWorldFlags(0x1000)) {
        if (++p->balance > 299)
            RepeatedEffect_SetState(p, 0, RepeatedEffect_TickUpperRecovery);
    }
}
