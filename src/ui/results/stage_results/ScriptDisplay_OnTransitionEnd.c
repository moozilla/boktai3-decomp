#include "global.h"
#include "ui/results/stage_results.h"

void ScriptDisplay_OnTransitionEnd(struct StageResults *results)
{
    if (results->entering != 0)
        results->entering = 0;
}
