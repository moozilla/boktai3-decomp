#include "stacked_actor.h"
struct StackedActor *Task_Create(u32, u32);
void Task_SetCallbacks(struct StackedActor *, s32 (*)(struct StackedActor *),
                       s32 (*)(struct StackedActor *));
s32 StackedActor_Initialize(struct StackedActor *, s32, s32);
void Task_Kill(struct StackedActor *);
s32 StackedActor_Tick(struct StackedActor *);
s32 StackedActor_Destroy(struct StackedActor *);
struct StackedActor *StackedActor_CreateFromScript(u32 a, u32 b)
{
    struct StackedActor *r = Task_Create(9, 0x388);
    if (r) {
        Task_SetCallbacks(r, StackedActor_Tick, StackedActor_Destroy);
        if (StackedActor_Initialize(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
