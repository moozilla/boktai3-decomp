#include "repeated_effect.h"
#define RepeatedEffect_CreateManager sub_0804FFEC
struct RepeatedEffectManager *Task_Create(s32, s32);
void Task_SetCallbacks(struct RepeatedEffectManager *, s32 (*)(struct RepeatedEffectManager *),
                       s32 (*)(struct RepeatedEffectManager *));
void Task_Kill(struct RepeatedEffectManager *);
struct RepeatedEffectManager *RepeatedEffect_CreateManager(void)
{
    struct RepeatedEffectManager *manager = Task_Create(9, sizeof(*manager));
    if (manager) {
        Task_SetCallbacks(manager, sub_0804FF48, sub_0804FF70);
        if (sub_0804FFDC(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
