#include "global.h"
#include "ui/results/stage_results.h"

void TextWindow_Clear(s32);

void StageResults_DestroyTextWindows(struct StageResults *results)
{
    s32 i;
    for (i = 0; i < 3; i++)
        TextWindow_Clear(results->textWindows[i]);
}
