#include "global.h"
s32 GameState_GetResultTimer(void);
s32 GameState_GetResultIndex(void);
void sub_0811D600(s32, s32);
s32 GameRecords_UpdateMinimum(s32, s32);
s32 StageResults_RecordStagePerformance(s32 p)
{
    s32 a = GameState_GetResultTimer();
    s32 b = GameState_GetResultIndex();
    s32 r;
    sub_0811D600(b, p);
    r = GameRecords_UpdateMinimum(b, a);
    if (r > 0)
        return 1;
    return -1;
}
