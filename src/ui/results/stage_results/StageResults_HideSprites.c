#include "global.h"
#include "ui/results/stage_results.h"

void CharacterModel_SetAnimationFlags(void *, s32);

void StageResults_HideSprites(struct StageResults *results)
{
    s32 i;
    for (i = 0; i < 23; i++)
        results->sprites[i].flags |= 1;
    CharacterModel_SetAnimationFlags(results->model, 1);
}
