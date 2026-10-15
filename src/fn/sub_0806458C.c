#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0806454C(u8 *, u32, u32);
void Task_Kill(u8 *);
void RoamingObjectB_Tick(void);
void sub_08064314(void);
u8 *sub_0806458C(u32 a, u32 b)
{
    u8 *r = Task_Create(8, 0x144);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void))RoamingObjectB_Tick, (void (*)(void))sub_08064314);
        if (sub_0806454C(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
