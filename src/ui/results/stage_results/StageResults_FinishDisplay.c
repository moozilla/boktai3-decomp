#include "global.h"
#include "ui/results/stage_results.h"
#include "engine/engine.h"

void Bg_ClearTilemap(s32);
void StageResults_HideSprites(struct StageResults *);
s32 GameState_GetResultIndex(void);

void StageResults_FinishDisplay(struct StageResults *results)
{
    s32 index;
    struct StageResultsScriptArgs args;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    StageResults_HideSprites(results);
    if (results->exitScript != 0) {
        index = GameState_GetResultIndex();
        args.count = 1;
        args.values = &index;
        Script_Call(results->exitScript, &args);
    }
}
