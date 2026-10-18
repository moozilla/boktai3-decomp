#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 VanargandProjectilePool_Init(void *, s32);
void Task_Kill(void *);
void sub_081B6FA8(void);
void VanargandProjectilePool_Destroy(void);

void *VanargandProjectilePool_Create(s32 a)
{
    void *r = Task_Create(9, 0x11c4);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081B6FA8, VanargandProjectilePool_Destroy);
        if (VanargandProjectilePool_Init(r, a) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
