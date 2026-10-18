#define gGameState              gUnk_02000710
#define Player_RecordInputTicks sub_0815EF6C
#include "global.h"
#include "player.h"
#include "game_state.h"
void sub_0815EF6C(struct Player *p)
{
    if (*p->inputKeys) {
        if (p->actionMode == 2) {
            if (gUnk_02000710->modeTwoInputTicks < 0x7fffffff)
                gUnk_02000710->modeTwoInputTicks++;
        } else if (p->actionMode == 1) {
            if (gUnk_02000710->modeOneInputTicks < 0x7fffffff)
                gUnk_02000710->modeOneInputTicks++;
        }
        {
            s32 *counter;
            s32 index = p->statModifierContext.statBOverrideEnabled;
            u8 *base = (u8 *)gUnk_02000710;
            counter = (s32 *)((index << 2) + (u32)base + 0x640);
            if (*counter < 0x7fffffff)
                ++*counter;
        }
    }
}
