#include "global.h"
#include "game_state.h"
static inline s32 ClampRecord(s32 value)
{
    s32 result = value;
    if (value > 60000)
        result = 60000;
    else if (value < 0)
        result = 0;
    return result;
}
s32 GameRecords_UpdateMinimum(u32 index, s32 value)
{
    s32 *record;
    if (index > 11 || value < 0)
        return -1;
    value = ClampRecord(value);
    {
        struct GameState *state = gGameState;
        record = (s32 *)(index * 4 + (u32)state + 0x7C8);
    }
    if (*record == -1 || *record > value)
        goto update;
    return -1;
update:
    *record = value;
    return 1;
}
