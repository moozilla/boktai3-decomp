#include "global.h"
#include "player.h"
#include "game_state.h"
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
        {
            s32 *counter;
            s32 index = p->statModifierContext.statBOverrideEnabled;
            u8 *base = (u8 *)gGameState;
            counter = (s32 *)((index << 2) + (u32)base + 0x640);
            if (*counter < 0x7fffffff)
                ++*counter;
        }
    }
}
