#include "stacked_actor.h"
#define StackedActor_CreateFromScript sub_0805376C
struct StackedActor *Task_Create(u32, u32);
void Task_SetCallbacks(struct StackedActor *, s32 (*)(struct StackedActor *),
                       s32 (*)(struct StackedActor *));
s32 sub_080532B0(struct StackedActor *, s32, s32);
void Task_Kill(struct StackedActor *);
s32 sub_08053178(struct StackedActor *);
s32 sub_080531BC(struct StackedActor *);
struct StackedActor *StackedActor_CreateFromScript(u32 a, u32 b)
{
    struct StackedActor *r = Task_Create(9, 0x388);
    if (r) {
        Task_SetCallbacks(r, sub_08053178, sub_080531BC);
        if (sub_080532B0(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
