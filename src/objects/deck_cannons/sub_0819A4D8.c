#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 CannonBattery_ReadOptions(void *);
void Task_Kill(void *);
void CannonBattery_Update(void);
void sub_0819A414(void);
void *CannonBattery_Create(u16 a)
{
    u32 *p = Task_Create(8, 0x1328);
    if (p) {
        p[6] = a;
        Task_SetCallbacks(p, CannonBattery_Update, sub_0819A414);
        if (CannonBattery_ReadOptions(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
