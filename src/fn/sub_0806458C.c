#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 RoamingObjectB_Initialize(u8 *, u32, u32);
void Task_Kill(u8 *);
void RoamingObjectB_Tick(void);
void RoamingObjectB_Destroy(void);
u8 *RoamingObjectB_Create(u32 a, u32 b)
{
    u8 *r = Task_Create(8, 0x144);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void))RoamingObjectB_Tick, (void (*)(void))RoamingObjectB_Destroy);
        if (RoamingObjectB_Initialize(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
