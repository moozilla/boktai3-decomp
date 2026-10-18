#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081A522C(void *, s32, s32, s32);
void Task_Kill(void *);
void Hresvelgr_UpdateMovingProjectile(void);
void Hresvelgr_DestroyMovingProjectile(void);

void *Hresvelgr_CreateMovingProjectile(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(10, 0x13c);
    if (r != 0) {
        Task_SetCallbacks(r, Hresvelgr_UpdateMovingProjectile, Hresvelgr_DestroyMovingProjectile);
        if (sub_081A522C(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
