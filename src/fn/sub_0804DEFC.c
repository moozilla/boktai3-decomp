#include "staged_effect.h"
struct StagedEffect *Task_Create(u32, u32);
void Task_SetCallbacks(struct StagedEffect *, s32 (*)(struct StagedEffect *),
                       s32 (*)(struct StagedEffect *));
s32 StagedEffect_Initialize(struct StagedEffect *, u32);
void Task_Kill(struct StagedEffect *);
s32 StagedEffect_Tick(struct StagedEffect *);
s32 StagedEffect_Destroy(struct StagedEffect *);
struct StagedEffect *StagedEffect_CreateFromScript(u32 script)
{
    struct StagedEffect *effect = Task_Create(8, sizeof(struct StagedEffect));
    if (effect) {
        Task_SetCallbacks(effect, StagedEffect_Tick, StagedEffect_Destroy);
        if (StagedEffect_Initialize(effect, script) < 0) {
            Task_Kill(effect);
            return 0;
        }
    }
    return effect;
}
