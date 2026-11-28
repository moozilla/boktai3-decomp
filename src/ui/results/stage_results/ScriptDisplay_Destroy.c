#include "global.h"
#include "ui/results/stage_results.h"

void StageResults_DestroyTextWindows(struct StageResults *);
void StageResults_DestroySprites(struct StageResults *);
void CharacterModel_RemoveAnimations(void *);

u32 ScriptDisplay_Destroy(struct StageResults *results)
{
    StageResults_DestroyTextWindows(results);
    StageResults_DestroySprites(results);
    CharacterModel_RemoveAnimations(results->model);
    return 1;
}
