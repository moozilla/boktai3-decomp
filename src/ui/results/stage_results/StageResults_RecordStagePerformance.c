#include "global.h"
s32 GameState_GetResultTimer(void);
s32 GameState_GetResultIndex(void);
void sub_0811D600(s32, s32);
s32 GameRecords_UpdateMinimum(s32, s32);
s32 StageResults_RecordStagePerformance(s32 rank)
{
    s32 time = GameState_GetResultTimer();
    s32 index = GameState_GetResultIndex();
    sub_0811D600(index, rank);
    if (GameRecords_UpdateMinimum(index, time) > 0)
        return 1;
    return -1;
}
