#include "global.h"
#include "ui/results/stage_results.h"

void StageResults_InitAnimationIds(struct StageResults *results)
{
    s32 i;
    results->spriteIds[0] = 0x34;
    results->spriteIds[1] = 0x39;
    results->spriteIds[2] = 0x3a;
    results->spriteIds[3] = 0x3b;
    results->spriteIds[6] = 0x48;
    results->spriteIds[4] = 0x21;
    results->spriteIds[5] = 0x22;
    results->spriteIds[7] = 0x3c;
    for (i = 8; i < 23; i++)
        results->spriteIds[i] = 0x3e;
}
