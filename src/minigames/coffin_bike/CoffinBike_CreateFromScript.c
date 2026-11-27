#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 CoffinBike_InitFromScript(u8 *);
void Task_Kill(u8 *);
void CoffinBike_UpdateTask(void);
void MotionActor_Destroy(void);

u8 *CoffinBike_CreateFromScript(void)
{
    u8 *p;
    p = Task_Create(6, 0x968);
    if (p != 0) {
        Task_SetCallbacks(p, CoffinBike_UpdateTask, MotionActor_Destroy);
        if (CoffinBike_InitFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
