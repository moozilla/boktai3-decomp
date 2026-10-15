#include "staged_effect.h"
#define StagedEffect_CreateFromScript sub_0804DEFC
struct StagedEffect *Task_Create(u32, u32);
void Task_SetCallbacks(struct StagedEffect *, s32 (*)(struct StagedEffect *),
                       s32 (*)(struct StagedEffect *));
s32 sub_0804DD48(struct StagedEffect *, u32);
void Task_Kill(struct StagedEffect *);
s32 sub_0804DC40(struct StagedEffect *);
s32 sub_0804DD10(struct StagedEffect *);
struct StagedEffect *StagedEffect_CreateFromScript(u32 script)
{
    struct StagedEffect *effect = Task_Create(8, sizeof(struct StagedEffect));
    if (effect) {
        Task_SetCallbacks(effect, sub_0804DC40, sub_0804DD10);
        if (sub_0804DD48(effect, script) < 0) {
            Task_Kill(effect);
            return 0;
        }
    }
    return effect;
}
