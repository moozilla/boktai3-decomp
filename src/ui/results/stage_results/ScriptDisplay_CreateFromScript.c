#include "global.h"
#include "ui/results/stage_results.h"
#include "engine/engine.h"

s32 MessagePanel_ConfigureFromScript(struct StageResults *);
u32 ScriptDisplay_Update(struct StageResults *);
u32 ScriptDisplay_Destroy(struct StageResults *);

struct StageResults *ScriptDisplay_CreateFromScript(void)
{
    struct StageResults *results = Task_Create(0xb, sizeof(struct StageResults));
    if (results != 0) {
        Task_SetCallbacks(results, (void (*)(void *))ScriptDisplay_Update,
                          (void (*)(void *))ScriptDisplay_Destroy);
        if (MessagePanel_ConfigureFromScript(results) < 0) {
            Task_Kill(results);
            return 0;
        }
    }
    return results;
}
