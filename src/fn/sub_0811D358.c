#include "global.h"
#include "engine.h"
void GameState_ClearSlots7A8FromScript(void)
{
    s32 i;
    for (i = 0; i <= 3; i++)
        *(s16 *)((u8 *)gGameState + i * 2 + 0x7A8) = -1;
}
