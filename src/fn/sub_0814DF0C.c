#include "global.h"
#include "player.h"
#include "game_state.h"

// MATCHING_CONCESSION: register-pinning
s32 Player_IsStatBBelow(struct Player *p, s32 threshold)
{
    s32 value;
    if (!p->statModifierContext.statBOverrideEnabled)
        goto current;
    asm("" ::: "r3");
    if (p->statModifierContext.statBOverrideMode == 7)
        value = gGameState->unk77C;
    else {
    current:
        value = p->currentStatB;
    }
    if (value < threshold)
        return 1;
    return 0;
}
