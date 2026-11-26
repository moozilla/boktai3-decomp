#include "objects/quest_barriers/quest_barrier.h"
struct BreakableProp *QuestBarrier_CreateFromScript(u32 a, u32 b)
{
    struct BreakableProp *p = Task_Create(8, 0xD0);
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))QuestBarrier_Tick,
                          (void (*)(void *))QuestBarrier_Destroy);
        if (QuestBarrier_InitializeFromScript(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
