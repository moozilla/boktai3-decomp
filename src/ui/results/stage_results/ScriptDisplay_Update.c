#include "global.h"
#include "ui/results/stage_results.h"

void Callback_InvokeActor(void *, void (*)(struct StageResults *));

u32 ScriptDisplay_Update(struct StageResults *results)
{
    if (results->update)
        Callback_InvokeActor(results, results->update);
    return 1;
}
