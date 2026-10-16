#include "global.h"
#include "engine.h"
/* Script: sets the four halfwords at game state +0x7A8 to -1. */
void GameState_ClearSlots7A8FromScript(void)
{
    s32 i;
    for (i = 0; i <= 3; i++)
        *(s16 *)((u8 *)gUnk_02000710 + i * 2 + 0x7A8) = -1;
}
