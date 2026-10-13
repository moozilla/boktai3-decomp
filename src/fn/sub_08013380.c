#include "breakable_barrier.h"

s32 BreakableBarriers_Initialize(void *);
u32 sub_0801324C(struct BreakableBarrierManager *);
u32 sub_08013270(struct BreakableBarrierManager *);

void *sub_08013380(void)
{
    void *obj;
    obj = Task_Create(8, 0x2FC);
    if (obj != 0) {
        Task_SetCallbacks(obj, sub_0801324C, sub_08013270);
        if (BreakableBarriers_Initialize(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
