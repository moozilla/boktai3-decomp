#include "global.h"
s32 GameState_GetResultTimer(void);
s32 GameState_GetResultIndex(void);
void sub_0811D600(s32, s32);
s32 sub_0811D5A8(s32, s32);
s32 StageResults_RecordStagePerformance(s32 p)
{
    s32 a = GameState_GetResultTimer();
    s32 b = GameState_GetResultIndex();
    s32 r;
    sub_0811D600(b, p);
    r = sub_0811D5A8(b, a);
    if (r > 0)
        return 1;
    return -1;
}
