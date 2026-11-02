#include "global.h"
#include "player.h"
#include "game_state.h"

void Player_SubtractStatB(struct Player *p, s32 amount)
{
    s32 value;
    if (!p->statModifierContext.statBOverrideEnabled)
        value = p->currentStatB;
    else if (p->statModifierContext.statBOverrideMode == 7)
        value = gGameState->unk77C;
    else
        value = p->currentStatB;
    if (value < amount)
        value = 0;
    else
        value -= amount;

    if (!p->statModifierContext.statBOverrideEnabled)
        p->currentStatB = value;
    else if (p->statModifierContext.statBOverrideMode == 7)
        gGameState->unk77C = value;
    else
        p->currentStatB = value;
}
