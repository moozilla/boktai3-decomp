#include "engine.h"
#include "game_state.h"
static inline void Increment(u16 *value)
{
    if ((s16)*value <= 9998)
        *value = *value + 1;
    else
        *value = 9999;
}
void SceneState_IncrementCounters(void)
{
    s32 next;
    Increment(&gGameState->counter87C);
    Increment(&gGameState->counter538);
    Increment(&gGameState->counter55E);
    next = gGameState->targetFlagBonuses[4] + 1;
    if (next > 9999)
        next = 9999;
    else if (next < 0)
        next = 0;
    gGameState->targetFlagBonuses[4] = next;
}
