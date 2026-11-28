#include "global.h"
#include "ui/results/stage_results.h"

s32 TextWindow_Create(s32, s32, s32, s32);

void StageResults_CreateTextWindows(struct StageResults *results)
{
    results->textWindows[0] = TextWindow_Create(1, 3, 0xa, 2);
    results->textWindows[1] = TextWindow_Create(0xd, 3, 4, 2);
    results->textWindows[2] = TextWindow_Create(0xd, 7, 0x10, 2);
}
