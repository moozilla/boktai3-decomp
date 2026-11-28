#include "global.h"
#include "player/player.h"
#include "system/game_state/game_state.h"
void Player_RecordInputTicks(struct Player *p)
{
    if (*p->inputKeys) {
        if (p->actionMode == 2) {
            if (gGameState->modeTwoInputTicks < 0x7fffffff)
                gGameState->modeTwoInputTicks++;
        } else if (p->actionMode == 1) {
            if (gGameState->modeOneInputTicks < 0x7fffffff)
                gGameState->modeOneInputTicks++;
        }
        if (*(gGameState->counters640 + p->statModifierContext.statBOverrideEnabled) < 0x7fffffff)
            ++*(gGameState->counters640 + p->statModifierContext.statBOverrideEnabled);
    }
}
