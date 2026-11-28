#include "global.h"
#include "ui/results/stage_results.h"

s32 Mod(s32, s32);

void StageResults_UpdateRecordBlink(struct StageResults *results)
{
    if (results->isRecord == 1) {
        if (Mod(results->blinkTimer, 20) == 0) {
            if (results->sprites[7].flags & 1)
                results->sprites[7].flags &= ~1;
            else
                results->sprites[7].flags |= 1;
        }
        results->blinkTimer++;
    } else {
        results->sprites[7].flags |= 1;
    }
}
