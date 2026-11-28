#include "global.h"
#include "ui/results/stage_results.h"

void Anim_Remove(void *);

void StageResults_DestroySprites(struct StageResults *results)
{
    s32 i;
    for (i = 0; i < 23; i++)
        Anim_Remove(&results->sprites[i]);
}
