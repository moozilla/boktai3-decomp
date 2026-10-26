#include "global.h"
#include "game_state.h"
void sub_08227E2C(s32 index)
{
    u32 base = (u32)gGameState;
    s16 *counter = (s16 *)((index * 2) + base + 0x87c);
    u16 v = *counter;
    if (*counter <= 9998)
        *counter = v + 1;
}
